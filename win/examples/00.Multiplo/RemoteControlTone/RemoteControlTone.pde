#include <IRremote.h>

//Use this example with the Multiplo IR 38 KHz sensor and a standard RC5 remote control:

IRrecv irReceiver(A0); //S0 input

void setup()
{
}

void loop()
{
   tone(SPEAKER, irReceiver.getIRRemoteCode()*440, 20); //440 Hz is the base freq.
   delay(100);
}
