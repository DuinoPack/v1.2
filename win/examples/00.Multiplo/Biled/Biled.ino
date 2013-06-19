void setup ()
{
  pinMode(BILED, OUTPUT);
}

#define GREEN_LED  1
#define RED_LED    0

void loop ()
{
  digitalWrite(BILED, GREEN_LED);
  delay(100);
  digitalWrite(BILED, RED_LED);
  delay(100);
}
