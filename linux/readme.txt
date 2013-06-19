1) Install Gcc-avr and avr-libc:

$ sudo apt-get install gcc-avr avr-libc

2) Install openjdk-6-jre if not installed already

$ sudo apt-get install openjdk-6-jre

3) Select correct JRE if more than one installed

$ sudo update-alternatives --config java

4) For using Multiplo CDC boards (i.e. DuinoBot 1.2 CDC, DuinoBot Kids 1.X or DuinoBot 2.X)
Connect board to usb and type:

$ dmesg

You will get somethig like:

[ 5029.492048] usb 4-1: >Product: DuinoBot V2.3 (CDC)
[ 5029.492051] usb 4-1: >Manufacturer: Multiplo (multiplo.org)
[ 5029.492053] usb 4-1: >SerialNumber: 75230313833351212180
[ 5029.533991] cdc_acm 4-1:1.0: >ttyACM0: USB ACM device
[ 5029.536273] usbcore: registered new interface driver cdc_acm
[ 5029.536278] cdc_acm: USB Abstract Control Model driver for USB modems and ISDN adapters

This means your board is at ttyACM0 serial port

5) For Multiplo HID boards (i.e. DuinoBot 1.2 HID)
Download and copy file 99-robotgroup.rules with the following command:

$ sudo cp 99-robotgroup.rules /etc/udev/rules.d

Then, restart udev

$ sudo stop udev
$ sudo start udev

6) Download and install DuinoPack.v1.0 IDE

$ tar xzvf duinopack-1.0-linux32.tar.gz

7) For Multiplo CDC boards (i.e. DuinoBot 1.2 CDC, DuinoBot Kids 1.X or DuinoBot 2.X) or for other arduino standard board
Change usb permissions by typing:

$ sudo chmod a+rw /dev/ttyACM0

(In this case our board is at ttyACM0. Remember to do this with your board connected and turned ON)

Repeat for other serial port board if neccesary, e.g.

$ sudo chmod a+rw /dev/ttyUSB0

8) Run DuinoPack IDE

$ ./arduino

or by double-cliking "arduino" file in installation directory and selecting "Run"

9) Select Board type and Serial Port, and load an example

10) Press the Upload button to flash the board
