
void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{
  for (int i=-99;i<99;i++){
    Screen.putNumber(i);
    delay(50);
  } 
}
