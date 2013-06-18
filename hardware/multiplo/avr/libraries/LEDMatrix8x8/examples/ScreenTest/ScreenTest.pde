
void setup()
{
}

void loop()
{
  for (int j=0;j<8;j++)
  {
    for (int i=0;i<8;i++)
	{
      Screen.setPixel(i,j);
      delay(50);
    }
  }
  
  for (int j=0;j<8;j++)
  {
    for (int i=0;i<8;i++)
	{
      Screen.clearPixel(i,j);
      delay(50);
    }
  }
}