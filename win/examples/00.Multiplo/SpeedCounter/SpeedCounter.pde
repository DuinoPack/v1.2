#include <DCMotor.h>

DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);

void setup()
{
  float speed = 0;
  motor0.setClockwise(false);

  while(speed < 100)
  {
    motor0.setSpeed(speed);
    motor1.setSpeed(-speed);
    delay(1000);

    speed = speed + 10;	
  }

  while(speed > 0)
  {
    motor0.setSpeed(speed);
    motor1.setSpeed(-speed);
    delay(1000);
    speed = speed - 10;
  }
}

void loop()
{}





