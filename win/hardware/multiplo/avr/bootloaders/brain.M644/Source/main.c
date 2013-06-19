/************************************************************/
/* Serial Bootloader for Multiplo(R) brains			      	*/
/*                                                        	*/
/* Tested with ATMega644p on a Multiplo.Brain.M644		  	*/
/* Should work with other mega's, specially on the M1284, 	*/
/* see code for details    									*/
/*                                                        	*/
/* main.cpp                                           		*/
/*                                                        	*/
/* 20100223: Modifications from J. da Silva Gillig			*/
/*			 (multiplo.org / robotgroup.com.ar):			*/
/*			- Now runs @18.432 MHz on a Mega644p.			*/
/*			- Serial works @115200 Bauds.					*/
/*			- Added battery check for Multiplo.Brains.		*/
/*			- Added Run-Button logic and eliminated the		*/
/*			  timeouts.										*/
/*			- Modified Monitor mode (here Interpreter).		*/
/*			- Modified the LED pin numbers.					*/
/*			- Modified LED behavior.						*/
/*			- Added some USART functions.					*/
/*			- Profuse use of __inline__ functions to reduce	*/
/*			  hex size (but not everywhere, in some cases 	*/
/*			  it has the counter effect).					*/
/*			- Now it's compiled with C++ (some minor		*/
/*			  changes were made for make this possible).	*/
/*			- Code not used in Multiplo brains has been 	*/
/*			  deprecated, to make the code simpler.			*/
/*			- Created Code::Blocks project.					*/
/*			- Modified indentation and some name functions, */
/*			  according to the Multiplo coding style.		*/
/*			- Now the bootloader needs a 2048 words (4096 	*/
/*			  bytes) boot sector.							*/
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
	##PENDIENTES / O PENDIENTES DE VERIFICACIÓN:

	- Hacer que al finalizar el bootloading ejecute automáticamente la aplicación:
		- Ver si se hace así o no, porque una cosa es para nuestro entorno (que puede abrir el puerto y enviar la orden para que
		ejecute), y otra cosa es para el entorno arduino, con avrdude, que no sé si se puede configurar para que ejecute
		automáticamente.
		¡¡YA ESTÁ!!
			- En el intérprete con "!!!" se puede hacer que el sistema:
				- Ejecute directamente.
				- Entre en bootloading.
				- Colocar el flag que le dice "Ejecutar al terminar" en true (que por defecto es true, lo que
				resuelve el problema con Arduino-IDE).
				- Colocar el flag que le dice "Ejecutar al terminar" en false:
					- De este modo, el XDF, entra PRIMERO al modo intérprete, luego coloca el flag en false o true, según sea la
					acción del usuario (ejecutar o sólo bajar el software y quedar a la espera -en cuyo caso podría resaltar el botón de
					ejecutar, si nos parece adeucado en su momento-), y finalmente entra en el modo bootloading con el flag seteado.
	- Agregar el chequeo de batería, al que no se llama en la versión actual, creo:
		- La idea es que si la batería está baja, queda titilando el led rojo, y si está listo para Run, queda el amarillo
		encendido de forma estable.
		- Si está en batería baja, envía un string junto al LED titilando, con la carga de batería, Y NO PERMITE BOOTLOADING, pero
		sí permite Run.
		- Terminar eso y probar el umbral correcto.
	- Arreglar el tema de que no siempre queda en bootloading (o sea se apaga el LED amarillo) cuando es un reset de Power.
		- Ver si se hace titilar un par de veces ambos LEDs cuando se pasa a Run state.
	- Botón de Run:
		- Agregar la protección para que no se pueda pasar a ejecución cuando está grabando.
	- Probar que ejecute la aplicación con una orden por puerto serie.
	- Agregar algún intérprete de comandos sencillo para que se pueda controlar al robot on line.
*/

/* some includes */
//##extern "C"
//{
	#include <inttypes.h>
	#include <avr/io.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>
	#include <avr/wdt.h>
	#include <util/delay.h>
	#include <stdlib.h>
//}


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
//##Test this:
#define BATT_MIN_LEVEL		128

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


__inline__ void flashYellowLED(uint8_t count)
{
	while (count--)
	{
		LED_PORT |= _BV(LED_YELLOW);
		_delay_ms(100);
		LED_PORT &= ~_BV(LED_YELLOW);
		_delay_ms(100);
	}
}


__inline__ void flashRedLED(uint8_t count)
{
	while (count--)
	{
		LED_PORT |= _BV(LED_RED);
		_delay_ms(100);
		LED_PORT &= ~_BV(LED_RED);
		_delay_ms(100);
	}
}


__inline__ void flashBothLEDs(uint8_t count)
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
		flashBothLEDs(3);

		//Shut down both LEDs:
		DDRB &= ~_BV(LED_RED);
		DDRB &= ~_BV(LED_YELLOW);

		//Start the application:
		appSectionJump();
	}
}


__inline__ void serial0PutChar(char ch)
{
	while (!(UCSR0A & _BV(UDRE0))) ;
	UDR0 = ch;
}


__inline__ void serial0PrintStr(const char *Str)
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


__inline__ void serial0PrintInt(int value)
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
		//##Verificar que funcione bien con el jumper de autorun:

		//##Por ahora, con presionar el botón basta, pero luego, si está grabando no debe funcionar:
			//##MUY IMPORTANTE: VER CÓMO HACER PARA QUE NO CORROMPA LA FLASH, CON UN FLAG DE QUE ESTÁ
			//ESCRIBIENDO, O ALGO ASÍ:
		if((BUTTON_PIN & _BV(RUN_BUTTON)) == 0)
			appStart();
	}

	LED_PORT &= ~_BV(LED_YELLOW);	// toggle LED to show activity - BBR/LF 10/3/2007
	return UDR0;
}


void getNch(uint8_t count)
{
	while(count--)
	{
		serial0GetChar();
	}
}

__inline__ void puthex(char ch)
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
		//##No me gusta, si dio muchos errores, prefiero resetear todo:
		if (++error_count == MAX_ERROR_COUNT)
			appStart();
	}
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


void  nothing_response(void)
{
	if (serial0GetChar() == ' ')
	{
		serial0PutChar(0x14);
		serial0PutChar(0x10);
	}
	else
	{
		//##No me gusta, si dio muchos errores, prefiero resetear todo:
		if (++error_count == MAX_ERROR_COUNT)
			appStart();
	}
}


__inline__ void usart0Init()
{
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


__inline__ void adcInit()
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


__inline__ uint8_t adcReadByte(uint8_t input)
{
	//##Verificar la inicialización del ADC:

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
	//##Se permitirá interpretar comandos, pero no escribir la memoria flash, a la que una batería baja puede
	//arruinar completamente. Si el usuario quiere grabar de todos modos, debe desconectar el sensor, por lo que sabrá
	//lo que está haciendo.

	NOTES:
		- Because the ADC0 pull-up is active, if the battery sensor is not conected, the Battery check will not report
		a low battery event, wich is de desired behavior.

		- Additionally, this functions sends the battery level through the USART, so the host software can show a
		message or a battery level indicator to the user.
*/
int readBatteryLevel()
{
	//##Probar que esto esté funcionando bien:

	uint16_t battLevel = 0;
	uint8_t i;

	//Dummy sensor reading:
	adcReadByte(0);

	//Reads 5 times the sensor, to reduce noise or any other bad reading:
	for (i=0; i<5; i++)
	{
		battLevel += adcReadByte(0);
	}

	//##Sends the battery raw value:
	//serial0PrintInt((int)(battLevel/5));
	return (int)(battLevel/5.0);
}


void startInterpreter()
{
	uint8_t ch;
	uint8_t addrl, addrh;

	serial0PrintStr("Multiplo.SerialInterpreter v1.0 (c)2010 Multiplo\n\r");
	//##while (true)
	for (;;)
	{
		serial0PutChar('\n');
		serial0PutChar('\r');
		serial0PutChar(':');
		serial0PutChar(' ');

		// Get character from UART
		ch = serial0GetChar();
		//##Acá se puede escribir el intérprete extra para el booloader de los Brains de Multiplo:
		//##Ver si es necesario el flag para la ejecución automática al terminar el loading, o si Arduino-IDE lo hace
		//automáticamente con el comando 'j':
		//##Probar!
		if(ch == 'j')
		{
			//Start the application:
			appStart();
		}
		else if(ch == 'b')
		{
			//Read the battery level (the function also sends the value through the USART):
			readBatteryLevel();
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
			//Toggle Yellow LED:
			if(bit_is_set(LED_PIN, LED_YELLOW))
			{
				LED_PORT &= ~_BV(LED_YELLOW);
				serial0PutChar('1');
			}
			else
			{
				LED_PORT |= _BV(LED_YELLOW);
				serial0PutChar('0');
			}
		}
		else if(ch == 'u')
		{
			//##Agregar a esto secuencia de escape:
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
		//##Here the interpreter can be extended:
		else if (ch=='x')
		{
			//Exits from the interpreter, and return to the Bootloader:
			return;
		}
	}
}


__inline__ void resetPorts()
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

	// Check if the WDT was used to reset, in which case we dont bootload and skip straight to the code. woot.
	if (! (ch &  _BV(EXTRF))) // if its a not an external reset...
		appStart();  // skip bootloader
#else
	asm volatile("nop\n\t");
#endif

	resetPorts();
	usart0Init();

	//Flash Boot LED:
	flashYellowLED(3);

	if (readBatteryLevel() > BATT_MIN_LEVEL)
	{
		//Main loop:
		//while(true)
		for (;;)
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
					//##No me gusta, si dio muchos errores, prefiero resetear todo:
					if (++error_count == MAX_ERROR_COUNT)
						appStart();
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
		#ifdef WATCHDOG_MODS
				// autoreset via watchdog (sneaky!)
				WDTCSR = _BV(WDE);
				while (1) ; // 16 ms
		#endif
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
						/* Should really add a wait for RWW section to be enabled, don't actually need it since we never */
						/* exit the bootloader without a power cycle anyhow */
					}
					serial0PutChar(0x14);
					serial0PutChar(0x10);
				}
				else
				{
					//##No me gusta, si dio muchos errores, prefiero resetear todo:
					if (++error_count == MAX_ERROR_COUNT)
						appStart();
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
					//##No me gusta, si dio muchos errores, prefiero resetear todo:
					if (++error_count == MAX_ERROR_COUNT)
						appStart();
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
			//##No me gusta, si dio muchos errores, prefiero resetear todo:
			else if (++error_count == MAX_ERROR_COUNT)
				appStart();
		} /* end of forever loop */
	}
	else
	{
		/* Low battery:
			- If the user wants to load an app at it's own risk, he will need to disconnect the battery sensor.
			- If the battery sesor is connected and the battery is low, the bootloader prevents the flash writing, but
			let's the user to execute the application, or to send commands to the interpreter through the USART.
		*/
		//Batt low:
		//PORTB &= ~_BV(LED_YELLOW); //Yellow LED off.
		flashRedLED(3);
		PORTB |= _BV(LED_RED); //Red LED on.
		startInterpreter();
	}

	//Defeinsive programming: the program never reaches this point (but just to be sure...):
	while(1) ;
	//for (;;) ;
}
