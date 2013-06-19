/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the HID class bootloader. This file contains the complete bootloader logic.
 */
 
 // Include for pins:
#include "LEDMatrix8x8Pins.h"
#include "pinsDuinoBot.h"

#include "BootloaderHID.h"

/** Flag to indicate if the bootloader should be running, or should exit and allow the application code to run
 *  via a soft reset. When cleared, the bootloader will abort, the USB interface will shut down and the application
 *  started via a forced watchdog reset.
 */
static bool RunBootloader = true;
#define memSize 8

uint8_t screenMem[memSize];     // screen memory
uint32_t CurrAddress;
uint8_t start = 0;

void initDisplay();

typedef void (*AppPtr_t)(void) ATTR_NO_RETURN;

#define _COLD_START_
void Bootloader_StartUserApp(void){      
		AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
		
		#ifdef _HOT_START_
			#warning "Using hot start mode. The bootloader wont close the CDC afeter downloading the software."
			/* Set GPIOR registers to a magic key that will be read by the user application */
			GPIOR0 = 0xDC;
			GPIOR1 = 0xA6;
		#elif defined(_COLD_START_)
			#warning "Using cold start mode. The bootloader will close the CDC afeter downloading the software."
			GPIOR0 = 0x00;
			GPIOR1 = 0x00;
		#else
			#error "Not bootloader switch mode selected"
		#endif
		
		MCUCR = (1 << IVCE);
		MCUCR = 0;
		
    AppStartPtr();
}

/** Main program entry point. This routine configures the hardware required by the bootloader, then continuously 
 *  runs the bootloader processing routine until instructed to soft-exit.
 */
int main(void)
{
	
	/* Setup hardware required for the bootloader */
	SetupHardware();
	
	// Configure DDR's for switch and Yellow Led
	LED_DDR |= LED_PIN;
	RUN_SW_DDR |= RUN_SW_PIN;
	RUN_SW_PORT |= RUN_SW_PIN;
	
	/* Enable global interrupts so that the USB stack can function */
	sei();
	
	screenMem[0] = 0b00000000;
	screenMem[1] = 0b00011000;
	screenMem[2] = 0b00011000;
	screenMem[3] = 0b00011000;
	screenMem[4] = 0b01111110;
	screenMem[5] = 0b00111100;
	screenMem[6] = 0b00011000;
	screenMem[7] = 0b00000000;

	while (RunBootloader){
		if (!(RUN_SW_PORTIN & RUN_SW_PIN)){
			RunBootloader = false;
		}
	  USB_USBTask();
	}
	
	start = 0;
	/* Disconnect from the host - USB interface will be reset later along with the AVR */
//	USB_Detach();
	//USB_Controller_Reset();
	//USB_ResetInterface();
	//_delay_ms(5000);
	USB_Disable();
	Bootloader_StartUserApp();
	
	/* Enable the watchdog and force a timeout to reset the AVR */
//	GPIOR0 = 0xDC;
	//GPIOR1 = 0xA6;

	//wdt_enable(WDTO_250MS);

	for (;;);
}

/** Configures all hardware required for the bootloader. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	if ((GPIOR0 == 0xDC) && (GPIOR1 == 0xA6)){
			AppPtr_t AppStartPtr = (AppPtr_t)0x0000;
			AppStartPtr();
	}
	
	/* Relocate the interrupt vector table to the bootloader section */
	MCUCR = (1 << IVCE);
	MCUCR = (1 << IVSEL);

	initDisplay();
	
	/* Initialize USB subsystem */
	USB_Init();
}

/** Event handler for the USB_ConfigurationChanged event. This configures the device's endpoints ready
 *  to relay data to and from the attached USB host.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Setup HID Report Endpoint */
	Endpoint_ConfigureEndpoint(HID_IN_EPNUM, EP_TYPE_INTERRUPT,
		                       ENDPOINT_DIR_IN, HID_IN_EPSIZE,
	                           ENDPOINT_BANK_SINGLE);
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	/* Ignore any requests that aren't directed to the HID interface */
	if ((USB_ControlRequest.bmRequestType & (CONTROL_REQTYPE_TYPE | CONTROL_REQTYPE_RECIPIENT)) !=
	    (REQTYPE_CLASS | REQREC_INTERFACE))
	{
		return;
	}

	/* Process HID specific control requests */
	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_SetReport:
			Endpoint_ClearSETUP();
			
			/* Wait until the command has been sent by the host */
			while (!(Endpoint_IsOUTReceived()));
		
			/* Read in the write destination address */
			#if (FLASHEND > 0xFFFF)
			uint32_t PageAddress = ((uint32_t)Endpoint_Read_16_LE() << 8);
			#else
			uint16_t PageAddress = Endpoint_Read_16_LE();
			#endif
			
			/* Check if the command is a program page command, or a start application command */
			#if (FLASHEND > 0xFFFF)
			if ((uint16_t)(PageAddress >> 8) == COMMAND_STARTAPPLICATION)
			#else
			if (PageAddress == COMMAND_STARTAPPLICATION)
			#endif
			{
				RunBootloader = false;
			}
			else
			{
			
				start = 1;
				
				/* Erase the given FLASH page, ready to be programmed */
				boot_page_erase(PageAddress);
				boot_spm_busy_wait();
				
				/* Write each of the FLASH page's bytes in sequence */
				for (uint8_t PageWord = 0; PageWord < (SPM_PAGESIZE / 2); PageWord++)				
				{
					/* Check if endpoint is empty - if so clear it and wait until ready for next packet */
					if (!(Endpoint_BytesInEndpoint()))
					{
						Endpoint_ClearOUT();
						while (!(Endpoint_IsOUTReceived()));
					}

					/* Write the next data word to the FLASH page */
					boot_page_fill(PageAddress + ((uint16_t)PageWord << 1), Endpoint_Read_16_LE());
				}

				/* Write the filled FLASH page to memory */
				boot_page_write(PageAddress);
				boot_spm_busy_wait();

				/* Re-enable RWW section */
				boot_rww_enable();
			}

			Endpoint_ClearOUT();

			Endpoint_ClearStatusStage();
			break;
	}
}


// =================================================================================
// Functions for the LedMatrix
// =================================================================================

/*
SIGNAL(TIMER0_OVF_vect)
{
	int i=0;
	static int ovfcount=0;
	uint8_t buffer;
	
	if ((ovfcount++)==3){
		
		buffer = screenMem[7];
		
		for (i=7;i>1;i--){
			screenMem[i] = screenMem[i-1];
		}
		screenMem[0] = buffer;
		
		ovfcount = 0;
	}
}
*/

 void initDisplay(){
	//Set columns as outputs
	C1_DDR |= (1<<C1);
	C2_DDR |= (1<<C2);
	C3_DDR |= (1<<C3);
	C4_DDR |= (1<<C4);
	C5_DDR |= (1<<C5);
	C6_DDR |= (1<<C6);
	C7_DDR |= (1<<C7);
	C8_DDR |= (1<<C8);

	//Set MUX selector lines for rows as outputs
	R0_DDR |= (1<<R0);
	R1_DDR |= (1<<R1);
	R2_DDR |= (1<<R2);

	//Clear all Columns	
	C1_PORT &=~ (1<<C1);
	C2_PORT &=~ (1<<C2);
	C3_PORT &=~ (1<<C3);
	C4_PORT &=~ (1<<C4);
	C5_PORT &=~ (1<<C5);
	C6_PORT &=~ (1<<C6);
	C7_PORT &=~ (1<<C7);
	C8_PORT &=~ (1<<C8);

	// Activate row 1
	R0_PORT &=~ (1<<R0);
	R1_PORT &=~ (1<<R1);
	R2_PORT &=~ (1<<R2);

	//Timer 1 setup

	// Clear registers
	TCCR1B = 0;
	TCCR1A = 0;

	// Set prescaler
	TCCR1B |= (1 << CS10);
	TCCR1B &=~(1 << CS11);
	TCCR1B &=~(1 << CS12);

	// Set Timer 1 in CTC MODE
	TCCR1A &=~(1<<WGM10);
	TCCR1A &=~(1<<WGM11);
	TCCR1B |= (1<<WGM12);
	TCCR1B &=~(1<<WGM13);

	// Compare Register
	OCR1A = 0x7FFF;

	// Maximum OCR1A value for smooth video: 0x9FFF
	// Any higher OCR1A value will show flickr on the screen
	// Any lower value will be nice, but maybe very CPU consuming

	//Enable timer 1 interrupt
	TIMSK1 |= (1 << OCIE1A);
}


ISR(TIMER1_COMPA_vect){	
		// TODO: This functions MUST BE optimized
    uint8_t row;
		static uint8_t activeRow=0;
		const uint8_t screenOrientation = 1;
	
		int i=0;
		static int ovfcount=0;
		uint8_t buffer;
		
		//=======================================================
		//Drawing
		if (start!=0){
			if ((ovfcount++)==25){
				
				buffer = screenMem[7];
				
				for (i=7;i>=1;i--){
					screenMem[i] = screenMem[i-1];
				}
				screenMem[0] = buffer;
				
				ovfcount = 0;
			}
		}
		//=======================================================
		
		// Display
    //Shut down all rows and columns
    //R1_PORT = 0xFF;
    C1_PORT &= ~(1<<C1);
    C2_PORT &= ~(1<<C2);
    C3_PORT &= ~(1<<C3);
    C4_PORT &= ~(1<<C4);
    C5_PORT &= ~(1<<C5);
    C6_PORT &= ~(1<<C6);
    C7_PORT &= ~(1<<C7);
    C8_PORT &= ~(1<<C8);

    //Next row
    activeRow = (activeRow+1) % memSize;
    
		if (screenOrientation==0)
		{
				row = screenMem[activeRow];
		}
		else if (screenOrientation==1)
		{
			row = screenMem[(memSize-1) - activeRow];
		}
	
    //Output all columns, switch leds on.

    //Column 1
    if ((row & 0x80) == 0x80)
    {
    	if (screenOrientation==0)
        	C1_PORT |= (1<<C1);
        else
        	C8_PORT |= (1<<C8);
    }
    //Column 2
    if ((row & 0x40) == 0x40)
    {
    	if (screenOrientation==0)
        	C2_PORT |= (1<<C2);
        else
        	C7_PORT |= (1<<C7);
    }
    //Column 3
    if ((row & 0x20) == 0x20)
    {
    	if (screenOrientation==0)
        	C3_PORT |= (1<<C3);
        else
        	C6_PORT |= (1<<C6);
    }
    //Column 4
    if ((row & 0x10) == 0x10)
    {
    	if (screenOrientation==0)
        	C4_PORT |= (1<<C4);
        else
        	C5_PORT |= (1<<C5);
    }
    //Column 5
    if ((row & 0x08) == 0x08)
    {
    	if (screenOrientation==0)
        	C5_PORT |= (1<<C5);
        else
        	C4_PORT |= (1<<C4);
    }
    //Column 6
    if ((row & 0x04) == 0x04)
    {
    	if (screenOrientation==0)
        	C6_PORT |= (1<<C6);
        else
        	C3_PORT |= (1<<C3);
    }
    //Column 7
    if ((row & 0x02) == 0x02)
    {
    	if (screenOrientation==0)
        	C7_PORT |= (1<<C7);
        else
        	C2_PORT |= (1<<C2);
    }
    //Column 8
    if ((row & 0x01) == 0x01)
    {
    	if (screenOrientation==0)
        	C8_PORT |= (1<<C8);
        else
        	C1_PORT |= (1<<C1);
    }


		if (activeRow & 0x01) {
			R0_PORT |= (1<<R0);
		}else{
			R0_PORT &=~(1<<R0);
		}

		if (activeRow & 0x02) {
			R1_PORT |= (1<<R1);
		}else{
			R1_PORT &=~(1<<R1);
		}

		if (activeRow & 0x04) {
			R2_PORT |= (1<<R2);
		}else{
			R2_PORT &=~(1<<R2);
		}
}