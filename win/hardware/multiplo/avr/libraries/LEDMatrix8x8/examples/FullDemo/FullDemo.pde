
void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{
 int i;
  Screen.print("Hello World!,");
  Screen.print('a');
  Screen.print(123456);
  Screen.print(-123456);
  Screen.print(3.1416);
  Screen.print(-45.1416);
  Screen.print("   ");
  
  Screen.print("ANIMATIONS:");
  Screen.scrollAnimation(invader0, invader1);
  
  delay(300);
  
  Screen.print(" ");
  
  Screen.print("IMAGES");
  
   Screen.showSprite(heartSmall);
  delay(300);
  Screen.showSprite(heartBig);
  delay(300);
  Screen.showSprite(heartSmall);
  delay(300);
  Screen.showSprite(heartBig);
  delay(300);
 
  Screen.print("   ");
  
  Screen.print("NORMAL OR INVERTED MODE:"); 
  
  Screen.showSprite(smile);
  
  for (i=0;i<3;i++){
    Screen.setInverted(screenNormal);
    delay(300);
    Screen.setInverted(screenInverted);
    delay(300);
  }
  Screen.setInverted(screenNormal);

  Screen.print("   ");
  
  Screen.setOrientation(screenUp);
  Screen.print("SCREEN UP    ");
  Screen.setOrientation(screenDown);
  Screen.print("SCREEN DOWN    ");
  Screen.setOrientation(screenUp);

  Screen.print("   ");
  
  Screen.print("Screen movement:");
  
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

  Screen.print("   ");

  Screen.print("Single char print:");
  for (char i=32;i<95;i++){
    Screen.putChar(i);
    delay(100);
  }
  
  Screen.print("   ");
  
  Screen.print("Number print:");
  for (int i=-99;i<99;i++){
    Screen.putNumber(i);
    delay(50);
  } 
  
  Screen.print("   ");
  
  Screen.print("Bars display:");
  
  for (i=0;i<8;i++){
    Screen.setBar(0,i);
    Screen.setBar(1,8-i);
    Screen.setBar(2,i);
    Screen.setBar(3,8-i);

    Screen.showBars();
    delay(100);
  }
  
  for (i=8;i>0;i--){
    Screen.setBar(0,i);
    Screen.setBar(1,8-i);
    Screen.setBar(2,i);
    Screen.setBar(3,8-i);

    Screen.showBars();
    delay(100);
  }
  
}

