int i = 0;

void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{
  uint8_t sensor0,sensor1;

  sensor0 = map(analogRead(0),0,1023,0,8);

  Screen.setBar(0,0);
  Screen.setBar(1,sensor0);
  Screen.setBar(2,sensor0);
  Screen.setBar(3,0);

  Screen.showBars();

  delay(100);  
}