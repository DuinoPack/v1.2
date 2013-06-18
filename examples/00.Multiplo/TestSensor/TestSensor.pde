void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Sensor 1: ");
  Serial.println(analogRead(1));
  
  delay(500);
}







