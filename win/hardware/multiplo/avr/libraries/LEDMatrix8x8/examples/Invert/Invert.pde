
void setup()
{
  Screen.setOrientation(screenUp);
  Screen.showSprite(smile);
}

void loop()
{
  Screen.setInverted(screenNormal);
  delay(300);
  Screen.setInverted(screenInverted);
  delay(300);
}
