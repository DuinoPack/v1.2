/*
 toneDelay Example:
 
 Based on toneMelody by Tom Igoe.
 The toneDelay function is an alternative to the tone function.
 Basically is the same except it is a blocking function, i.e. it does not use a timer for frequency generation
 
 Author: Federico Lanza
 
*/

#include "Arduino.h"
#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

void setup() {
  pinMode(SPEAKER, OUTPUT);
}

void loop() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    toneDelay(SPEAKER, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    int pauseBetweenNotes = noteDuration * 0.8;
    delay(pauseBetweenNotes);
  }
  
  delay(1000);
}
