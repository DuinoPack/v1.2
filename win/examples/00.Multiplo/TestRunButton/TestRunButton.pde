int buttonState;

void setup() {
  Serial.begin(9600);
  pinMode(RUN_SW, INPUT_PULLUP);
}

void loop() {
  buttonState = digitalRead(RUN_SW);
  Serial.println(buttonState);
  delay(100);
}