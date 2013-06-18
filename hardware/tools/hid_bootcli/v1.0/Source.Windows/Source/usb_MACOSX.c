
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include "usb_XXX.h"
/****************************************************************/
/*                                                              */
/*             USB Access - Apple's IOKit, Mac OS-X             */
/*                                                              */
/****************************************************************/

#if defined(OS_MACOSX)

// http://developer.apple.com/technotes/tn2007/tn2187.html
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDDevice.h>

struct usb_list_struct {
	IOHIDDeviceRef ref;
	int pid;
	int vid;
	struct usb_list_struct *next;
};

static struct usb_list_struct *usb_list=NULL;
static IOHIDManagerRef hid_manager=NULL;

int printf_verbose(const char *format, ...);

void attach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	CFTypeRef type;
	struct usb_list_struct *n, *p;
	int32_t pid, vid;

	if (!dev) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDVendorIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &vid)) return;
	type = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDProductIDKey));
	if (!type || CFGetTypeID(type) != CFNumberGetTypeID()) return;
	if (!CFNumberGetValue((CFNumberRef)type, kCFNumberSInt32Type, &pid)) return;
	n = (struct usb_list_struct *)malloc(sizeof(struct usb_list_struct));
	if (!n) return;
	//printf("attach callback: vid=%04X, pid=%04X\n", vid, pid);
	n->ref = dev;
	n->vid = vid;
	n->pid = pid;
	n->next = NULL;
	if (usb_list == NULL) {
		usb_list = n;
	} else {
		for (p = usb_list; p->next; p = p->next) ;
		p->next = n;
	}
}

void detach_callback(void *context, IOReturn r, void *hid_mgr, IOHIDDeviceRef dev)
{
	struct usb_list_struct *p, *tmp, *prev=NULL;

	p = usb_list;
	while (p) {
		if (p->ref == dev) {
			if (prev) {
				prev->next = p->next;
			} else {
				usb_list = p->next;
			}
			tmp = p;
			p = p->next;
			free(tmp);
		} else {
			prev = p;
			p = p->next;
		}
	}
}

void init_hid_manager(void)
{
	CFMutableDictionaryRef dict;
	IOReturn ret;

	if (hid_manager) return;
	hid_manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	if (hid_manager == NULL || CFGetTypeID(hid_manager) != IOHIDManagerGetTypeID()) {
		if (hid_manager) CFRelease(hid_manager);
		printf_verbose("no HID Manager - maybe this is a pre-Leopard (10.5) system?\n");
		return;
	}
	dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (!dict) return;
	IOHIDManagerSetDeviceMatching(hid_manager, dict);
	CFRelease(dict);
	IOHIDManagerScheduleWithRunLoop(hid_manager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	IOHIDManagerRegisterDeviceMatchingCallback(hid_manager, attach_callback, NULL);
	IOHIDManagerRegisterDeviceRemovalCallback(hid_manager, detach_callback, NULL);
	ret = IOHIDManagerOpen(hid_manager, kIOHIDOptionsTypeNone);
	if (ret != kIOReturnSuccess) {
		IOHIDManagerUnscheduleFromRunLoop(hid_manager,
			CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		CFRelease(hid_manager);
		printf_verbose("Error opening HID Manager");
	}
}

static void do_run_loop(void)
{
	while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true) == kCFRunLoopRunHandledSource) ;
}

IOHIDDeviceRef open_usb_device(int vid, int pid)
{
	struct usb_list_struct *p;
	IOReturn ret;

	init_hid_manager();
	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		if (p->vid == vid && p->pid == pid) {
			ret = IOHIDDeviceOpen(p->ref, kIOHIDOptionsTypeNone);
			if (ret == kIOReturnSuccess) return p->ref;
		}
	}
	return NULL;
}

void close_usb_device(IOHIDDeviceRef dev)
{
	struct usb_list_struct *p;

	do_run_loop();
	for (p = usb_list; p; p = p->next) {
		if (p->ref == dev) {
			IOHIDDeviceClose(dev, kIOHIDOptionsTypeNone);
			return;
		}
	}
}

static IOHIDDeviceRef iokit_teensy_reference = NULL;

int teensy_open(void)
{
	teensy_close();
	iokit_teensy_reference = open_usb_device(0x16C0, 0x0478);

	if (!iokit_teensy_reference)
		iokit_teensy_reference = open_usb_device(0x03eb, 0x2067);

	if (!iokit_teensy_reference) return 0;
	return 1;
}

int teensy_write(void *buf, int len, double timeout)
{
	IOReturn ret;

	// timeouts do not work on OS-X
	// IOHIDDeviceSetReportWithCallback is not implemented
	// even though Apple documents it with a code example!
	// submitted to Apple on 22-sep-2009, problem ID 7245050
	if (!iokit_teensy_reference) return 0;
	ret = IOHIDDeviceSetReport(iokit_teensy_reference,
		kIOHIDReportTypeOutput, 0, buf, len);
	if (ret == kIOReturnSuccess) return 1;
	return 0;
}

void teensy_close(void)
{
	if (!iokit_teensy_reference) return;
	close_usb_device(iokit_teensy_reference);
	iokit_teensy_reference = NULL;
}

int hard_reboot(void)
{
	IOHIDDeviceRef rebootor;
	IOReturn ret;

	rebootor = open_usb_device(0x16C0, 0x0477);

	if (!rebootor)
		rebootor = open_usb_device(0x03eb, 0x2067);

	if (!rebootor) return 0;
	ret = IOHIDDeviceSetReport(rebootor,
		kIOHIDReportTypeOutput, 0, (uint8_t *)("reboot"), 6);
	close_usb_device(rebootor);
	if (ret == kIOReturnSuccess) return 1;
	return 0;
}

#endif

