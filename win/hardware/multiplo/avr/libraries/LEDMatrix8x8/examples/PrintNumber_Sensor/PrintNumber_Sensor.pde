
void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{
  int value;

  value = analogRead(0);
  value = map(value, 0,1023,-100,100);
  
  Screen.putNumber(value);
  delay(100);
}
