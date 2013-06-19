
void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{
  int i;
  
  Screen.showSprite(smile);  
  delay(500);
  for (i=0;i<7;i++){
    Screen.scrollLeft(1);
    delay(200);
  }
  
  Screen.showSprite(smile);  
  delay(500);
  for (i=0;i<7;i++){
    Screen.scrollRight(1);
    delay(200);
  }
  
    
  Screen.showSprite(smile);  
  delay(500);
  for (i=0;i<7;i++){
    Screen.scrollUp(1);
    delay(200);
  }
  
   
  Screen.showSprite(smile);  
  delay(500);
  for (i=0;i<7;i++){
    Screen.scrollDown(1);
    delay(200);
  }
  
}
