#include <DCMotor.h>

DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);

void setup()
{
  motor0.setSpeed(-50.0);
  motor1.setSpeed(50.0);
  delay(5000);

  motor0.brake();
  motor1.brake();
}

void loop()
{ }

