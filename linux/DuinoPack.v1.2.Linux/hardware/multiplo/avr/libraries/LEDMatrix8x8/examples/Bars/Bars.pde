int i = 0;

void setup()
{
  Screen.setOrientation(screenUp);
}

void loop()
{

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
