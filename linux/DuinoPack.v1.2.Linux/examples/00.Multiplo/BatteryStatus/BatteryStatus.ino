void setup()
{
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(BATTERY);
  Serial.println(sensorValue);
  delay(100);
}
