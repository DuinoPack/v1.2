/*
  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef CDCSerial_h
#define CDCSerial_h

#include <inttypes.h>

#include "Print.h"

struct ring_buffer;

class CDCSerial : public Print
{
  private:    
	bool running;
	ring_buffer *_rx_buffer;
	
  public:
	
    CDCSerial(ring_buffer *rx_buffer);
	void store(char c);
    void begin(long);
	void end();
    uint8_t available(void);
    int read(void);
    void flush(void);
	// This function where overrided and re written for the virtual serial
    virtual void write(uint8_t);
	virtual void write(const uint8_t *buffer, size_t size);
	virtual void write(const char *str);
    using Print::write; // pull in write(str) and write(buf, size) from Print
};

extern CDCSerial Serial;

#endif
