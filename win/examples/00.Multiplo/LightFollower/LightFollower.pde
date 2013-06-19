#include <DCMotor.h>

DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);

void setup()
{
  motor0.setClockwise(false);
}

void loop()
{
  if(analogRead(1)>300)
  {
    motor0.setSpeed(40);
    motor1.setSpeed(40); 
  }
  else if(analogRead(2)>300)
  {
    motor0.setSpeed(70);
    motor1.setSpeed(20);
  }
  else if(analogRead(3)>300)
  {
    motor0.setSpeed(20);
    motor1.setSpeed(70);
  }
  else
  {
    motor0.setSpeed(0);
    motor1.setSpeed(0);
  }
}










