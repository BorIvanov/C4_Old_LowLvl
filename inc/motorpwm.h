#ifndef motorpwm_H
#define motorpwm_H

void initMotorPWM();
void motorPWMSetDutyCycle(int motor1, int motor2);
void EnableMotors(int x, int z, int servo);
void DisableMotors(int x, int z, int servo);
void EnableXMotors();
void EnableZMotors();
void EnableServoMotors();
void MotorPWMduty(double motx, double motz);
void DisableXMotors(void);
void DisableZMotors(void);
void DisableServoMotors(void);

#endif
