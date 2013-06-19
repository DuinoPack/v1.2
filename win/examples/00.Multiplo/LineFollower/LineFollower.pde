#include <DCMotor.h>

#define TRIGGER		600
#define MOTOR_SPEED	70.0

float s0, s1;
DCMotor motor0(M0_EN, M0_D0, M0_D1);
DCMotor motor1(M1_EN, M1_D0, M1_D1);
  
void setup()
{
  Serial.begin(115200);
  motor1.setClockwise(false);
  motor0.setSpeed(MOTOR_SPEED);
  motor1.setSpeed(MOTOR_SPEED);
}

void loop()
{
  //Read sensors:
  s0 = analogRead(0);
  s1 = analogRead(1);
  
  /*
  //Debug:
  Serial.print("s0 = ");
  Serial.print(s0);
  Serial.print("/ s1 = ");
  Serial.println(s1);
  */

  if ( (s0<TRIGGER) && (s1<TRIGGER) )  //00
  {
    motor0.setSpeed(MOTOR_SPEED);
    motor1.setSpeed(MOTOR_SPEED);
  }
  else if ( (s0<TRIGGER) && (s1>TRIGGER) ) //01
  {
    motor0.setSpeed(0.1*MOTOR_SPEED);
    motor1.setSpeed(MOTOR_SPEED);
  }
  else if( (s0>TRIGGER) && (s1<TRIGGER) ) //10
  {
    motor0.setSpeed(MOTOR_SPEED);
    motor1.setSpeed(0.1*MOTOR_SPEED);
  }
  //11: Keeps last state.
}
