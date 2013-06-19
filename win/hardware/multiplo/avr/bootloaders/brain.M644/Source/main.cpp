/************************************************************/
/* Serial Bootloader for Multiplo(R) brains			      	*/
/*                                                        	*/
/* Tested with ATMega644p on a Multiplo.Brain.M644		  	*/
/* Should work with other mega's, specially on the M1284, 	*/
/* see code for details    									*/
/*                                                        	*/
/* main.cpp                                           		*/
/*                                                        	*/
/* 20101004: Modifications from J. da Silva Gillig			*/
/*			 ( http://multiplo.org							*/
/*			   http://robotgroup.com.ar ):					*/
/*			- Deprecated battery check for Multiplo.Brains. */
/* 20100406: Modifications from J. da Silva Gillig			*/
/*			 ( http://multiplo.org							*/
/*			   http://robotgroup.com.ar ):					*/
/*			- Now runs @18.432 MHz on a Mega644p.			*/
/*			- Serial works @115200 Bauds.					*/
/*			- Added battery check for Multiplo.Brains.		*/
/*			- Added Run-Button logic and eliminated the		*/
/*			  timeouts.										*/
/*			- Modified the way the bootloader detects the	*/
/*			  reset-source, to avoid false app start on		*/
/*			  POR (Power On Reset).							*/
/*			- Modified Monitor mode (here Interpreter).		*/
/*			- Modified the LED pin numbers.					*/
/*			- Modified LED behavior.						*/
/*			- Now the LED pins aren't negated.				*/
/*			- Added some USART functions.					*/
/*			- Split some code in functions, and manually	*/
/*			  optimized ising __inline__ to reduce hex		*/
/*			  (some times the code is smaller with inline, 	*/
/*			  some times not).								*/
/*			  it has the counter effect).					*/
/*			- Some behavior changed: If the error count 	*/
/*			  reaches MAX_ERROR_COUNT, starts the 			*/
/*			  interpreter, not the applications. This is 	*/
/*			  safer, at least when working with physical	*/
/*			  robots.										*/
/*			- Deprecated all forward declarations, so now	*/
/*			  it's a strict top-down program.				*/
/*			- Now it's compiled with C++ (some minor		*/
/*			  changes were made for make this possible). In	*/
/*			  this particular case, this seems to reduce 	*/
/*			  hex size too.									*/
/*			- Code not used in Multiplo brains has been 	*/
/*			  deprecated, to make the code simpler.			*/
/*			- Modified indentation and some name functions, */
/*			  according to the Multiplo coding style.		*/
/*			- Now the bootloader needs a 4096 words (8192 	*/
/*			  bytes) boot sector. Yes, I know it's a lot,	*/
/*			  but we really need these features.			*/
/*			- Created Code::Blocks project.					*/
/* 20090325: BBR applied ADABoot fixes to the 328 enabled 	*/
/*           bootloader code of Arduino-0014. Many ADABOOT	*/
/*           included by DAM under WATCHDOG Mods #ifdef   	*/
/* 20080813: BBR tweaked flash_led()  timing and          	*/
/*           shortened timeout n Makefile                 	*/
/* 20071005: BBR tweaked delay values in flash_led()      	*/
/* 20071004: B Riley - modified flash_led() amd its invo- 	*/
/*           cation to give unique signature and version  	*/
/*           for ADABOOT. Search on ADABOOT for changes   	*/
/* 20071003: Limore Fried and B Riley, hacks to timeout,  	*/
/*           upload progress detector on Dig13, BL abort  	*/
/*           of serial port                               	*/
/* 20070626: hacked for Arduino Diecimila (which auto-    	*/
/*           resets when a USB connection is made to it)  	*/
/*           by D. Mellis                                 	*/
/* 20060802: hacked for Arduino by D. Cuartielles         	*/
/*           based on a previous hack by D. Mellis        	*/
/*           and D. Cuartielles                           	*/
/*                                                        	*/
/* Monitor and debug functions were added to the original 	*/
/* code by Dr. Erik Lins, chip45.com. (See below)        	*/
/*                                                        	*/
/* Thanks to Karl Pitrich for fixing a bootloader pin     	*/
/* problem and more informative LED blinking!             	*/
/*                                                        	*/
/* For the latest version see:                            	*/
/* http://www.chip45.com/                                 	*/
/*                                                        	*/
/* -------------------------------------------------------- */
/*                                                        	*/
/* based on stk500boot.c                                  	*/
/* Copyright (c) 2003, Jason P. Kyle                      	*/
/* All rights reserved.                                   	*/
/* see avr1.org for original file and information         	*/
/*                                                        	*/
/* This program is free software; you can redistribute it 	*/
/* and/or modify it under the terms of the GNU General    	*/
/* Public License as published by the Free Software       	*/
/* Foundation; either version 2 of the License, or        	*/
/* (at your option) any later version.                    	*/
/*                                                        	*/
/* This program is distributed in the hope that it will   	*/
/* be useful, but WITHOUT ANY WARRANTY; without even the  	*/
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   	*/
/* PARTICULAR PURPOSE.  See the GNU General Public        	*/
/* License for more details.                              	*/
/*                                                        	*/
/* You should have received a copy of the GNU General     	*/
/* Public License along with this program; if not, write  	*/
/* to the Free Software Foundation, Inc.,                 	*/
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 	*/
/*                                                        	*/
/* Licence can be viewed at                               	*/
/* http://www.fsf.org/licenses/gpl.txt                    	*/
/*                                                        	*/
/* Target = Atmel AVR m128,m64,m32,m16,m8,m162,m163,m169, 	*/
/* m8515,m8535. ATmega161 has a very small boot block so  	*/
/* isn't supported.                                       	*/
/*                                                        	*/
/* Tested with m168                                       	*/
/************************************************************/

/* $Id$ */

/*
	##PENDIENTES / O PENDIENTES DE VERIFICACI�N:

	- ##PROBAR EL SISTEMA DE UMBRAL DE BATER�A CON EL N10
		- Probar tambi�n el sistema de regulaci�n del umbral desde el puerto, con el N10.
	- Se agreg� que EL UMBRAL DE BATER�A SEA CONFIGURABLE DESDE EL INT�RPRETE:
		- As� el usuario puede personalizar esto desde XDF, o algo as�, ya que antes de llamar al avrdude, el
		software puede configurar esto, quiz� porque el el usuario inform� de otro tipo de bater�a o placa de power.
	- Ver qu� oc
	- DOCUMENTAR: Cuando se utiliza el m�dulo Comm.RS232, porque pareciera que no se recupera del autoreset y que quedara
	el pin de DTR en low, o algo as� ��MEDIR!!
		->> ##ES UN BUG DEL XDFTerminal, que coloca al DTR en low ni bien se abre el Comm o cuando se vuelve
		a seleccionar el baudrate.
		##��CORREGIR ESO YA!!
		->> ##Pero OJO: en Arduino IDE el DTR se queda abajo cuando se abre un terminal. Esto es un problema y
		puede seguirlo siendo en Processing, etc., cuando se usa el m�dulo Comm.RS232, puesto que en el circuito
		original de Arduino, la l�nea DTR est� conectada a Reset por medio de un peque�o capacitor, para s�lo
		resetear en el pulso inicial, cuando DTR cambia.
	- DOCUMENTAR: El sistema de execution flag (variable runAfterBootloading), s�lo funciona si el software que abre
	el puerto para grabar al micro no vuelve a hacer un reset, por ejemplo, por culpa del avrdude. ##Ver si hay forma
	de hacer que el avrdude no env�e el pulso de reset al puerto.
	- Ver si se puede reducir a�n un poco el tama�o del bootloader.
		- Quiz� eliminando PrintInt y la libc, por ejemplo, quedemos a tiro para los 4096 bytes.
	- DOCUMENTAR: Valor de bater�a por USART:
		- Para que no interfiera con el avrdude, quiz� lo mejor sea no enviar nada, y que el software tipo XDF y
		Minibloq ingresen al int�rprete antes, al cual de todos modos deben ingresar para establecer si har�n un
		download solamente, o un Run.
	- AVRDude + IDFS + Bootloader para Minibloq:
		- Quiz� convenga reescribir el c�digo de stk500, tomando el de avrdude, para manejar yo la apertura del puerto
		virtual, etc., para asegurar compatibilidad con nuestros m�dulos Bluetooth.
	- DOCUMENTAR: Autorun:
		- En el int�rprete con "!!!" se puede hacer que el sistema:
			- Ejecute directamente.
			- Entre en bootloading.
			- Colocar el flag que le dice "Ejecutar al terminar" en true (que por defecto es true, lo que
			resuelve el problema con Arduino-IDE).
			- Colocar el flag que le dice "Ejecutar al terminar" en false:
				- De este modo, el XDF, entra PRIMERO al modo int�rprete, luego coloca el flag en false o true, seg�n sea la
				acci�n del usuario (ejecutar o s�lo bajar el software y quedar a la espera -en cuyo caso podr�a resaltar el bot�n de
				ejecutar, si nos parece adeucado en su momento-), y finalmente entra en el modo bootloading con el flag seteado.
	- ##Agregar alg�n int�rprete de comandos sencillo para que se pueda controlar al robot on line.
*/

/* some includes */
extern "C"
{
	#include <inttypes.h>
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#include <util/delay.h>
	#include <stdlib.h>
}


// SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio
// never allow AVR Studio to do an update !!!!
#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10

/* define various device id's */
/* manufacturer byte is always the same */
#define SIG1	0x1E	// Yep, Atmel is the only manufacturer of AVR micros.  Single source :(

#if defined __AVR_ATmega128__
#define SIG2	0x97
#define SIG3	0x02
#define PAGE_SIZE	0x80U	//128 words

#elif defined __AVR_ATmega64__
#define SIG2	0x96
#define SIG3	0x02
#define PAGE_SIZE	0x80U	//128 words

#elif defined __AVR_ATmega32__
#define SIG2	0x95
#define SIG3	0x02
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega16__
#define SIG2	0x94
#define SIG3	0x03
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega8__
#define SIG2	0x93
#define SIG3	0x07
#define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega88__
#define SIG2	0x93
#define SIG3	0x0a
#define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega168__
#define SIG2	0x94
#define SIG3	0x06
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega328P__
#define SIG2	0x95
#define SIG3	0x0F
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega162__
#define SIG2	0x94
#define SIG3	0x04
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega163__
#define SIG2	0x94
#define SIG3	0x02
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega169__
#define SIG2	0x94
#define SIG3	0x05
#define PAGE_SIZE	0x40U	//64 words

#elif defined __AVR_ATmega8515__
#define SIG2	0x93
#define SIG3	0x06
#define PAGE_SIZE	0x20U	//32 words

#elif defined __AVR_ATmega8535__
#define SIG2	0x93
#define SIG3	0x08
#define PAGE_SIZE	0x20U	//32 words

#elif defined(__AVR_ATmega644P__)
#define SIG2	0x96
#define SIG3	0x0A
#define PAGE_SIZE		0x080U   //128 words
#define PAGE_SIZE_BYTES	0x100U   //256 bytes

#elif defined(__AVR_ATmega644__)
#define SIG2	0x96
#define SIG3	0x09
#define PAGE_SIZE		0x080U   //128 words
#define PAGE_SIZE_BYTES	0x100U   //256 bytes

#elif defined(__AVR_ATmega324P__)
#define SIG2	0x95
#define SIG3	0x08
#define PAGE_SIZE		0x080U   //128 words
#define PAGE_SIZE_BYTES	0x100U   //256 bytes

#endif


///////////////////////////////////
// Configuration:
///////////////////////////////////
#define WATCHDOG_MODS	1

#define LED_DDR		DDRB
#define LED_PORT	PORTB
#define LED_PIN		PINB
#define LED_YELLOW	PINB0
#define LED_RED		PINB1

#define BUTTON_DDR	DDRB
#define BUTTON_PORT	PORTB
#define BUTTON_PIN	PINB
#define RUN_BUTTON	PINB2

//Battery minimal ADC (8 bits) reading:
#define DEFAULT_BATTERY_MIN_LEVEL		128

// 20070707: hacked by David A. Mellis - after this many errors give up and launch application:
#define MAX_ERROR_COUNT 5

#ifndef BAUD_RATE
#define BAUD_RATE   115200
#endif


union address_union
{
	uint16_t word;
	uint8_t  byte[2];
} address;

union length_union
{
	uint16_t word;
	uint8_t  byte[2];
} length;

struct flags_struct
{
	unsigned eeprom : 1;
	unsigned rampz  : 1;
} flags;

uint8_t buff[256];
uint8_t address_high;
uint8_t error_count = 0;
uint8_t batteryMinLevel = DEFAULT_BATTERY_MIN_LEVEL;
bool runAfterBootloading = true;


void __inline__ flashYellowLED(uint8_t count)
{
	while (count--)
	{
		LED_PORT |= _BV(LED_YELLOW);
		_delay_ms(100);
		LED_PORT &= ~_BV(LED_YELLOW);
		_delay_ms(100);
	}
}


void __inline__ flashRedLED(uint8_t count)
{
	while (count--)
	{
		LED_PORT |= _BV(LED_RED);
		_delay_ms(100);
		LED_PORT &= ~_BV(LED_RED);
		_delay_ms(100);
	}
}


void __inline__ flashBothLEDs(uint8_t count)
{
	while (count--)
	{
		LED_PORT |= _BV(LED_YELLOW) | _BV(LED_RED);
		_delay_ms(100);
		LED_PORT &= ~(_BV(LED_YELLOW) | _BV(LED_RED));
		_delay_ms(100);
	}
}

void (*appSectionJump)(void) = 0x0000;


void appStart(void)
{
	//Start the application, but only executes de app if the Flash is programmed already
	//(this is to prevent the CPU to go to a possible undefined state):
	if(pgm_read_byte_near(0x0000) != 0xFF)
	{
		//Restore the CPU I/Os, but keeps the motor drivers with Enable bit = 0 and the LEDs as outputs, but off:
		DDRA = 0;
		PORTA = 0;

		DDRB = 0;
		PORTB = 0;

		DDRC = 0;
		PORTC = 0;

		DDRB = 0;
		PORTB = 0;

		//Motors:
		DDRD = 0;
		PORTD = 0;
		DDRD |= _BV(4) | _BV(5) | _BV(6) | _BV(7);				//Motor 0 enable and direction bits / Motor 1 enable.
		PORTD &= ~(_BV(4) | _BV(5) | _BV(6) | _BV(7));
		DDRC |= _BV(6) | _BV(7);								//Motor 1 direction bits.
		PORTC &= ~(_BV(6) | _BV(7));

		//Red and Yellow LEDs as output (but off):
		DDRB |= _BV(LED_RED);
		DDRB |= _BV(LED_YELLOW);

		//Flash both LEDs to show the app is starting:
		//flashBothLEDs(3);
		//Strobes Red LED 3 times:
		_delay_ms(100);
		LED_PORT |= _BV(LED_RED);
		_delay_ms(25);
		LED_PORT &= ~_BV(LED_RED);
		_delay_ms(100);
		LED_PORT |= _BV(LED_RED);
		_delay_ms(25);
		LED_PORT &= ~_BV(LED_RED);
		_delay_ms(100);
		LED_PORT |= _BV(LED_RED);
		_delay_ms(25);
		LED_PORT &= ~_BV(LED_RED);

		//Shut down both LEDs:
		LED_PORT &= ~_BV(LED_RED);
		LED_PORT &= ~_BV(LED_YELLOW);

		//Start the application:
		appSectionJump();
	}
}


void __inline__ serial0PutChar(char ch)
{
	while (!(UCSR0A & _BV(UDRE0))) ;
	UDR0 = ch;
}


void __inline__ serial0PrintStr(const char *Str)
{
    unsigned int    i = 0;
    char            c = Str[0];

	//c = 0;

    while(c)
    {
        serial0PutChar(c);
        i++;
        c = Str[i];
    }
}


void __inline__ serial0PrintInt(int value)
{
	char	str[50];

	itoa(value, str, 10);
	serial0PrintStr(str);
	//serial0PutChar('\r');
}



char serial0GetChar(void)
{
	LED_PORT |= _BV(LED_YELLOW);	// toggle LED to show activity - BBR/LF 10/3/2007

	while(!(UCSR0A & _BV(RXC0)))
	{
		/* ##DOCUMENTAR: Con bot�n de autorun, no arranca el bootloading:
			Este es el comportamiento esperado, porque de otro modo habr�a que a�adir un time-out o algo
			as� al Run, puesto que de entrar al bootloader y encontrar el Run button presionado, tendr�a que preguntar
			si hay data en el puerto, o algo as�. Sin embargo, cuando la placa se usa en autorun, es probable que
			no est� conectada a nada que le pueda bajar software, ya que de estarlo, �ste "master" podr�a enviar
			la orden de ejecutar por la UASRT. Esto as� es, por ahora, una soluci�n de compromiso. Si la placa
			entra al bootloader y est� el jumper de Autorun colocado, entonces no se puede hacer bootloading, sino
			que ejecuta directamente.
		*/

		//##Verificar que funcione bien con el jumper de autorun:
		if((BUTTON_PIN & _BV(RUN_BUTTON)) == 0)
		{
			/*
			##En realidad, la idea va a ser proteger de que no corte una escritura de p�gina de la flash, no que no
			interrumpa la grabaci�n general, porque no hay mucha forma de darse cuenta de que termin� de grabar,
			salvo que llegue un comando del avrdude (tipo 'Q', o que llegue un reset f�sico del entorno, o que el
			usuario resetee, en cuyo caso tampoco se puede proteger mucho nada, porque aborta a la mitad de la
			escritura), as� que como esto est� en el getchar, si presionan el Run button, y el bootloader lo lee, es
			porque est� aqu� intentando obtener nuevos caracteres de la USART (�ste es el �nico lugar donde se lee
			el Run button):
			*/
			//##Parece que todo esto es encesaro para habilitar la secci�n RWW:
				/* Leave programming mode  */
				//nothing_response(); ##Esto no, porque ser�a una llamada recursiva!
		#ifdef WATCHDOG_MODS
				//serial0PrintStr("\r\nRun\r\n"); //##Debug.
				// autoreset via watchdog (sneaky!)
				WDTCSR = _BV(WDE);
				while (1) ; // 16 ms
		#endif
		}
	}

	LED_PORT &= ~_BV(LED_YELLOW);	// toggle LED to show activity - BBR/LF 10/3/2007
	return UDR0;
}


void getNch(uint8_t count)
{
	while(count--)
		serial0GetChar();
}

void __inline__ puthex(char ch)
{
	char ah;

	ah = ch >> 4;
	if(ah >= 0x0a)
		ah = ah - 0x0a + 'a';
	else
		ah += '0';

	ch &= 0x0f;
	if(ch >= 0x0a)
		ch = ch - 0x0a + 'a';
	else
		ch += '0';

	serial0PutChar(ah);
	serial0PutChar(ch);
}


char  gethexnib(void)
{
	char a;
	a = serial0GetChar();
	serial0PutChar(a);
	if(a >= 'a')
	{
		return (a - 'a' + 0x0a);
	}
	else if(a >= '0')
	{
		return(a - '0');
	}
	return a;
}

char gethex(void)
{
	return (gethexnib() << 4) + gethexnib();
}


uint8_t __inline__ adcReadByte(uint8_t input)
{
	//##Verificar la inicializaci�n del ADC:
	ADCSRA &= ~_BV(ADSC);

	if (input < 8)
	{
		ADMUX = _BV(REFS0) | _BV(ADLAR);
		ADMUX |= input;
	}
    ADCSRA |= _BV(ADSC);
	loop_until_bit_is_clear(ADCSRA, ADSC);

	return (ADCH);
}


/**
	//##Se permitir� interpretar comandos, pero no escribir la memoria flash, a la que una bater�a baja puede
	//arruinar completamente. Si el usuario quiere grabar de todos modos, debe desconectar el sensor, por lo que sabr�
	//lo que est� haciendo.

	NOTES:
		- Because the ADC0 pull-up is active, if the battery sensor is not conected, the Battery check will not report
		a low battery event, wich is the desired behavior.

		- Additionally, this functions sends the battery level through the USART, so the host software can show a
		message or a battery level indicator to the user.
*/
unsigned int readBatteryLevel()
{
	//##Probar que esto est� funcionando bien:

	uint16_t battLevel = 0;

	//Dummy sensor reading:
	adcReadByte(0);

	//Reads 5 times the sensor, to reduce noise or any other bad reading:
	for (uint8_t i=0; i<5; i++)
	{
		battLevel += adcReadByte(0);
	}

	return (int)(battLevel/5.0);
}


void startInterpreter()
{
	uint8_t ch;
	uint8_t addrl, addrh;

	serial0PrintStr("Multiplo.SerialInterpreter v1.0 (c)2010 Multiplo\n\r");
	while (true)
	{
		serial0PutChar('\n');
		serial0PutChar('\r');
		serial0PutChar(':');
		serial0PutChar(' ');

		// Get character from UART
		ch = serial0GetChar();
		//##Ac� se puede escribir el int�rprete extra para el booloader de los Brains de Multiplo:
		//##Ver si es necesario el flag para la ejecuci�n autom�tica al terminar el loading, o si Arduino-IDE lo hace
		//autom�ticamente con el comando 'j':
		//##Probar!
		if(ch == 'b')
		{
			//Read the battery level (the function also sends the value through the USART):
			serial0PrintStr("Batt:");
			serial0PrintInt(readBatteryLevel());
			//puthex((char)readBatteryLevel());
			serial0PutChar('\n');
			serial0PutChar('\r');
		}
		if(ch == 'B')
		{
			//Sets the new battery threshold:
			char c[4];
			c[0] = serial0GetChar();
			c[1] = serial0GetChar();
			c[2] = serial0GetChar();
			c[3] = NULL;
			//No error checking, so the input number must be correct:
			batteryMinLevel = atoi(c);
			serial0PrintStr("\n\rbattMin=");
			serial0PrintInt(batteryMinLevel);
		}
		else if(ch == 'j')
		{
			//Start the application:
			appStart();
		}
		else if(ch == 'r')
		{
			//Read byte from address:
			ch = serial0GetChar();
			serial0PutChar(ch);
			addrh = gethex();
			addrl = gethex();
			serial0PutChar('=');
			ch = *(uint8_t *)((addrh << 8) + addrl);
			puthex(ch);
		}
		else if(ch == 't')
		{
			//Toggle Red LED:
			//NOTE: The yeloow LED is used by the serial0GetChar function, so it's not possible to
			//control it from the interpreter.
			if(bit_is_set(LED_PIN, LED_RED))
			{
				LED_PORT &= ~_BV(LED_RED);
				serial0PutChar('0');
			}
			else
			{
				LED_PORT |= _BV(LED_RED);
				serial0PutChar('1');
			}
		}
		else if(ch == 'u')
		{
			//##Agregar a esto secuencia de escape:
			//	##DOCUMENTAR: Mejor no: Que el usuario o el IDE reseteen y listo!
			//Read from uart and echo back:
			for(;;)
				serial0PutChar(serial0GetChar());
		}
		else if(ch == 'w')
		{
			//Write a byte to address:
			ch = serial0GetChar();
			serial0PutChar(ch);
			addrh = gethex();
			addrl = gethex();
			ch = serial0GetChar();
			serial0PutChar(ch);
			ch = gethex();
			*(uint8_t *)((addrh << 8) + addrl) = ch;
		}
		else if (ch=='x')
		{
			//Resets the execution flag:
			runAfterBootloading = false;
			serial0PutChar('0');
		}
		else if (ch=='X')
		{
			//Sets the execution flag. The execution flag system only works if the circuit is not reset by the avrdude
			//after leaving the interpreter to enter programming mode:
			runAfterBootloading = true;
			serial0PutChar('1');
		}
		else if (ch=='Q')
		{
			//Exits from the interpreter, and return to the Bootloader:
			return;
		}
		//Here the interpreter can be extended...
		else
			continue;
	}
}


void  nothing_response(void)
{
	if (serial0GetChar() == ' ')
	{
		serial0PutChar(0x14);
		serial0PutChar(0x10);
	}
	else
	{
		//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
		if (++error_count == MAX_ERROR_COUNT)
			startInterpreter();
	}
}


void byte_response(uint8_t val)
{
	if (serial0GetChar() == ' ')
	{
		serial0PutChar(0x14);
		serial0PutChar(val);
		serial0PutChar(0x10);
	}
	else
	{
		//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
		if (++error_count == MAX_ERROR_COUNT)
			startInterpreter();
	}
}


void __inline__ usart0Init()
{
	//F_CPU=18432000UL
	// 115.2 Kbd @ 18432 kHz
	// Error=0.0%
	// 8N1
  	UBRR0H = 0x00;
  	UBRR0L = 0x09;
  	UCSR0A = 0x0;
  	UCSR0B = _BV(RXEN0) | _BV(TXEN0); //Rx Enabled + Tx Enabled
  	//UCSR0C = _BV(URSEL) | _BV(UCSZ00) | _BV(UCSZ01);
  	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

	/* ##Sanguino orginal code: In future versions it will be desirable to test this code and use it, wich calculates
	automatically the baudrate:
	UBRR0L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
	UBRR0H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
	*/

	/* Enable internal pull-up resistor on pin D0 (RX), in order
	to supress line noise that prevents the bootloader from
	timing out (DAM: 20070509) */
	DDRD &= ~_BV(PIND0);
	PORTD |= _BV(PIND0);
}


void __inline__ adcInit()
{
	/*
	REFS1 = 0, REFS0 = 1: Internal AREF=AVCC with external capacitor at AREF pin. Internal voltage turned OFF.
	ADLAR = 1: 	Left adjust the 10 bits result (to use the 8 MSBs).
	*/
	ADMUX = _BV(REFS0) | _BV(ADLAR);

	/*
	ADEN = 1: ADC enabled.
	ADPS2 = ADPS1 = 1: Ck/64.
	ADFR = 0: Single conversion mode.
	ADSC = 1: ADC hardware initialization.
	ADFR = 1: Free running mode.
	ADIE = 0: ADC interrupt disabled.
	*/
	//ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADFR) | _BV(ADSC) | _BV(ADIE);
	ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADSC);

	//All ADC bits enabled:
	DIDR0 = 0;

	//Free Running:
	ADCSRB &= ~(_BV(ADTS2) | _BV(ADTS1) | _BV(ADTS0));
}


void __inline__ resetPorts()
{
	//Shutdown Motors:
	DDRD = 0;
	PORTD = 0;
	DDRD |= _BV(4) | _BV(5) | _BV(6) | _BV(7);				//Motor 0 enable and direction bits / Motor 1 enable.
	PORTD &= ~(_BV(4) | _BV(5) | _BV(6) | _BV(7));
	DDRC |= _BV(6) | _BV(7);								//Motor 1 direction bits.
	PORTC &= ~(_BV(6) | _BV(7));

	//Sensors as hi-z inputs:
	//DDRA &= ~(_BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) | _BV(7));
	//PORTA &= ~(_BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) | _BV(7));
	DDRA = 0;
	PORTA = _BV(0); //Only bit 0 (ADC0) with its pull-up on.

	//Comm1 as hi-z inputs:
	DDRD &= ~(_BV(2) | _BV(3));
	PORTD &= ~(_BV(2) | _BV(3));

	//Expansion pads, ISP and JTAG pins as hi-z inputs:
	DDRB &= ~_BV(3);
	PORTB &= ~_BV(3);
	DDRC &= ~(_BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5));
	PORTC &= ~(_BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5));

	//Red (Red) LED as output (but off):
	DDRB |= _BV(LED_RED);
	PORTB &= ~(_BV(LED_RED));

	//Yellow (Red) LED as output (but off):
	DDRB |= _BV(LED_YELLOW);
	PORTB &= ~(_BV(LED_YELLOW));

	//Run button as input with pull-up:
	DDRB &= ~_BV(RUN_BUTTON);
	PORTB |= _BV(RUN_BUTTON);
}

int main(void)
{
	uint8_t ch,ch2;
	uint16_t w;
#ifdef ADABOOT
	uint8_t	firstcharzero = 0; 	// lady ada hack - BBR
#endif

#ifdef WATCHDOG_MODS
	ch = MCUSR;
	MCUSR = 0;

	WDTCSR |= _BV(WDCE) | _BV(WDE);
	WDTCSR = 0;

/* ##2010.04.06: Original code:
	// Check if the WDT was used to reset, in which case we dont bootload and skip straight to the code. woot.
	if (! (ch &  _BV(EXTRF))) // if its a not an external reset...
		appStart();  // skip bootloader
*/
	// Check if the WDT was used to reset, in which case we dont bootload and skip straight to the code. woot.
	if (ch &  _BV(WDRF)) // if it's a watchdog reset...
		appStart();  // skip bootloader

#else
	asm volatile("nop\n\t");
#endif

	resetPorts();
	adcInit();
	usart0Init();

	//Flash Boot LED:
	flashYellowLED(3);

//  Deprecated 2010.10.04): battery check
//	if (readBatteryLevel() > batteryMinLevel)
//	{
		//Main loop:
		while(true)
		{
			/* get character from UART */
			ch = serial0GetChar();

			/* A bunch of if...else if... gives smaller code than switch...case ! */
			if(ch=='0')
			{
				// Hello is anyone home?
				nothing_response();
			}
			else if(ch=='1')
			{
				// Request programmer ID
				// Not using PROGMEM string due to boot block in m128 being beyond 64kB boundry
				// Would need to selectively manipulate RAMPZ, and it's only 9 characters anyway so who cares.
				if (serial0GetChar() == ' ')
				{
					serial0PutChar(0x14);
					serial0PutChar('A');
					serial0PutChar('V');
					serial0PutChar('R');
					serial0PutChar(' ');
					serial0PutChar('I');
					serial0PutChar('S');
					serial0PutChar('P');
					serial0PutChar(0x10);
				}
				else
				{
					//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
					if (++error_count == MAX_ERROR_COUNT)
						startInterpreter();
				}
			}
			else if(ch=='@')
			{
				/* AVR ISP/STK500 board commands  DON'T CARE so default nothing_response */
				ch2 = serial0GetChar();
				if (ch2>0x85) serial0GetChar();
				nothing_response();
			}
			else if(ch=='A')
			{
				/* AVR ISP/STK500 board requests */
				ch2 = serial0GetChar();
				if(ch2==0x80)
					byte_response(HW_VER);		// Hardware version
				else if(ch2==0x81)
					byte_response(SW_MAJOR);	// Software major version
				else if(ch2==0x82)
					byte_response(SW_MINOR);	// Software minor version
				else if(ch2==0x98)
					byte_response(0x03);		// Unknown but seems to be required by avr studio 3.56
				else
					byte_response(0x00);				// Covers various unnecessary responses we don't care about
			}
			else if(ch=='B')
			{
				/* Device Parameters  DON'T CARE, DEVICE IS FIXED  */
				getNch(20);
				nothing_response();
			}
			else if(ch=='E')
			{
				/* Parallel programming stuff  DON'T CARE  */
				getNch(5);
				nothing_response();
			}
			else if(ch=='P' || ch=='R')
			{
				/* P: Enter programming mode  */
				/* R: Erase device, don't care as we will erase one page at a time anyway.  */
				nothing_response();
			}
			else if(ch=='Q')
			{
				/* Leave programming mode  */
				nothing_response();

				//Only executes (reset via watchdog) if runAfterBootloading is true. Other way, waits for the Run
				//button to be pressed, or for the interpreter to start the app:
				if (runAfterBootloading)
				{
			#ifdef WATCHDOG_MODS
					// autoreset via watchdog (sneaky!)
					WDTCSR = _BV(WDE);
					while (1) ; // 16 ms
			#endif
				}
			}
			else if(ch=='U')
			{
				/* Set address, little endian. EEPROM in bytes, FLASH in words  */
				/* Perhaps extra address bytes may be added in future to support > 128kB FLASH.  */
				/* This might explain why little endian was used here, big endian used everywhere else.  */
				address.byte[0] = serial0GetChar();
				address.byte[1] = serial0GetChar();
				nothing_response();
			}
			else if(ch=='V')
			{
				/* Universal SPI programming command, disabled.  Would be used for fuses and lock bits.  */
				if (serial0GetChar() == 0x30)
				{
					serial0GetChar();
					ch = serial0GetChar();
					serial0GetChar();
					if (ch == 0)
						byte_response(SIG1);
					else if (ch == 1)
						byte_response(SIG2);
					else
						byte_response(SIG3);
				}
				else
				{
					getNch(3);
					byte_response(0x00);
				}
			}
			else if(ch=='d')
			{
				/* Write memory, length is big endian and is in bytes  */
				length.byte[1] = serial0GetChar();
				length.byte[0] = serial0GetChar();
				flags.eeprom = 0;
				if (serial0GetChar() == 'E')
					flags.eeprom = 1;
				for (w=0;w<length.word;w++)
				{
					buff[w] = serial0GetChar();	  // Store data in buffer, can't keep up with serial data stream whilst programming pages
				}
				if (serial0GetChar() == ' ')
				{
					if (flags.eeprom)
					{
						//Write to EEPROM one byte at a time
						address.word <<= 1;
						for(w=0;w<length.word;w++)
						{
							while(EECR & (1<<EEPE)) ;
							EEAR = (uint16_t)(void *)address.word;
							EEDR = buff[w];
							EECR |= (1<<EEMPE);
							EECR |= (1<<EEPE);
							address.word++;
						}
					}
					else
					{
						//Write to FLASH one page at a time
						if (address.byte[1]>127)
							address_high = 0x01;	//Only possible with m128, m256 will need 3rd address byte. FIXME
						else
							address_high = 0x00;
						address.word = address.word << 1;//address * 2 -> byte location
						if ((length.byte[0] & 0x01))
							length.word++;	//Even up an odd number of bytes
						cli();				//Disable interrupts, just to be sure
						// HACKME: EEPE used to be EEWE
						while(bit_is_set(EECR,EEPE)) ;	//Wait for previous EEPROM writes to complete
						asm volatile(
							 "clr	r17		\n\t"	//page_word_count
							 "lds	r30,address	\n\t"	//Address of FLASH location (in bytes)
							 "lds	r31,address+1	\n\t"
							 "ldi	r28,lo8(buff)	\n\t"	//Start of buffer array in RAM
							 "ldi	r29,hi8(buff)	\n\t"
							 "lds	r24,length	\n\t"	//Length of data to be written (in bytes)
							 "lds	r25,length+1	\n\t"
							 "length_loop:		\n\t"	//Main loop, repeat for number of words in block
							 "cpi	r17,0x00	\n\t"	//If page_word_count=0 then erase page
							 "brne	no_page_erase	\n\t"
							 "wait_spm1:		\n\t"
							 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
							 "andi	r16,1           \n\t"
							 "cpi	r16,1           \n\t"
							 "breq	wait_spm1       \n\t"
							 "ldi	r16,0x03	\n\t"	//Erase page pointed to by Z
							 "sts	%0,r16		\n\t"
							 "spm			\n\t"
							 "wait_spm2:		\n\t"
							 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
							 "andi	r16,1           \n\t"
							 "cpi	r16,1           \n\t"
							 "breq	wait_spm2       \n\t"

							 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
							 "sts	%0,r16		\n\t"
							 "spm			\n\t"
							 "no_page_erase:		\n\t"
							 "ld	r0,Y+		\n\t"	//Write 2 bytes into page buffer
							 "ld	r1,Y+		\n\t"

							 "wait_spm3:		\n\t"
							 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
							 "andi	r16,1           \n\t"
							 "cpi	r16,1           \n\t"
							 "breq	wait_spm3       \n\t"
							 "ldi	r16,0x01	\n\t"	//Load r0,r1 into FLASH page buffer
							 "sts	%0,r16		\n\t"
							 "spm			\n\t"

							 "inc	r17		\n\t"	//page_word_count++
							 "cpi r17,%1	        \n\t"
							 "brlo	same_page	\n\t"	//Still same page in FLASH
							 "write_page:		\n\t"
							 "clr	r17		\n\t"	//New page, write current one first
							 "wait_spm4:		\n\t"
							 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
							 "andi	r16,1           \n\t"
							 "cpi	r16,1           \n\t"
							 "breq	wait_spm4       \n\t"
							 "ldi	r16,0x05	\n\t"	//Write page pointed to by Z
							 "sts	%0,r16		\n\t"
							 "spm			\n\t"
							 "wait_spm5:		\n\t"
							 "lds	r16,%0		\n\t"	//Wait for previous spm to complete
							 "andi	r16,1           \n\t"
							 "cpi	r16,1           \n\t"
							 "breq	wait_spm5       \n\t"
							 "ldi	r16,0x11	\n\t"	//Re-enable RWW section
							 "sts	%0,r16		\n\t"
							 "spm			\n\t"
							 "same_page:		\n\t"
							 "adiw	r30,2		\n\t"	//Next word in FLASH
							 "sbiw	r24,2		\n\t"	//length-2
							 "breq	final_write	\n\t"	//Finished
							 "rjmp	length_loop	\n\t"
							 "final_write:		\n\t"
							 "cpi	r17,0		\n\t"
							 "breq	block_done	\n\t"
							 "adiw	r24,2		\n\t"	//length+2, fool above check on length after short page write
							 "rjmp	write_page	\n\t"
							 "block_done:		\n\t"
							 "clr	__zero_reg__	\n\t"	//restore zero register
							 : "=m" (SPMCSR) : "M" (PAGE_SIZE) : "r0","r16","r17","r24","r25","r28","r29","r30","r31"
							 );

						//##Ver si esto es afectado por el bot�n de Run:

						/* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
						/* exit the bootloader without a power cycle anyhow */
					}
					serial0PutChar(0x14);
					serial0PutChar(0x10);
				}
				else
				{
					//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
					if (++error_count == MAX_ERROR_COUNT)
						startInterpreter();
				}
			}
			else if(ch=='t')
			{
				/* Read memory block mode, length is big endian.  */
				length.byte[1] = serial0GetChar();
				length.byte[0] = serial0GetChar();
				address.word = address.word << 1;	        // address * 2 -> byte location
				if (serial0GetChar() == 'E')
					flags.eeprom = 1;
				else
					flags.eeprom = 0;
				if (serial0GetChar() == ' ')
				{
					// Command terminator
					serial0PutChar(0x14);
					for (w=0;w < length.word;w++)
					{
						// Can handle odd and even lengths okay
						if (flags.eeprom)
						{
							// Byte access EEPROM read
							while(EECR & (1<<EEPE)) ;
							EEAR = (uint16_t)(void *)address.word;
							EECR |= (1<<EERE);
							serial0PutChar(EEDR);
							address.word++;
						}
						else
						{
							if (!flags.rampz)
								serial0PutChar(pgm_read_byte_near(address.word));
							address.word++;
						}
					}
					serial0PutChar(0x10);
				}
			}
			else if(ch=='u')
			{
				/* Get device signature bytes  */
				if (serial0GetChar() == ' ')
				{
					serial0PutChar(0x14);
					serial0PutChar(SIG1);
					serial0PutChar(SIG2);
					serial0PutChar(SIG3);
					serial0PutChar(0x10);
				}
				else
				{
					//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
					if (++error_count == MAX_ERROR_COUNT)
						startInterpreter();
				}
			}
			else if(ch=='v')
			{
				/* Read oscillator calibration byte */
				byte_response(0x00);
			}
			// Check for three times exclamation mark pressed:
			else if(ch=='!')
			{
				ch = serial0GetChar();
				if(ch=='!')
				{
					ch = serial0GetChar();
					if(ch=='!')
					{
						startInterpreter();
					}
				}
			}
			//If the error count reaches MAX_ERROR_COUNT, starts the interpreter, not the applications (this is safer):
			else if (++error_count == MAX_ERROR_COUNT)
				startInterpreter();
		} /* end of forever loop */
    /*
	}
	else
	{
		// Low battery:
		//	- If the user wants to load an app at it's own risk, he will need to disconnect the battery sensor.
		//	- If the battery sensor is connected and the battery is low, the bootloader prevents the flash writing, but
		//	let's the user to execute the application, or to send commands to the interpreter through the USART.
		//Batt low:
		//PORTB &= ~_BV(LED_YELLOW); //Yellow LED off.
		flashRedLED(8);
		PORTB |= _BV(LED_RED); //Red LED on.
		startInterpreter();
	}
    */

	//Defensive programming: the program never reaches this point (but just to be sure...):
	while (true) ;
}
