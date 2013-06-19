/*

Example for IR + Motors + Servo + Tone + Serial
Author: Federico Lanza
ingfedericolanza [at] gmail [.] com

This example demostrates how to use Serial, IR sensor, Motors, Servo and play a tone through speaker/buzzer
all at the same time.

Nov 2012

*/ 

#include <IRremote.h>
#include <Servo.h>
#include <DCMotor.h>

// Declare Servo
Servo MiServo;
// Motors
DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);
// IR receiver
IRrecv irReceiver(A0);

int LastValue, Received, RecChar;

void setup()
{
  // Init Serial Port
   Serial.begin(9600);
   LastValue = Received = 0;
   // Attach Servo to S5
   MiServo.attach(A5);
   // Set Buzzer/Speaker to Output
   pinMode(SPEAKER, OUTPUT);
}

void loop()
{
  Received = irReceiver.getIRRemoteCode();
  if (Received != LastValue) {
    // Something different received from IR, show it
    if (Received) {
      Serial.print("IR data received: ");
      Serial.println(Received);
    }
    LastValue = Received;
  }
  if (Serial.available() > 0) {
    // Char received from Serial Port (Monitor), Show it and process
    RecChar = Serial.read();
    Serial.print("I received: ");
    Serial.println(RecChar, DEC);
    if (RecChar == 65) {
      // If it is 'A' then turn motors on for 1 sec
      motor0.setClockwise(false);
      motor0.setSpeed(70);
      motor1.setSpeed(-70);
      delay (1000);
      motor0.setSpeed(0);
      motor1.setSpeed(0);
    } else if ((RecChar - '0') >= 0 && (RecChar - '0') <= 9) {
      // We received a number, set servo to a relative position and play a tone
      MiServo.write((RecChar - '0') * 10);
      // We used toneDelay, because Timer 1 is used in DuinoBot 1.2 for both Servo and Tone
      // and in DuinoBot 2.3 Timer 2 is used for both IR and Tone
      toneDelay(SPEAKER, (RecChar - '0') * 440, 100);
    }
  }
}
