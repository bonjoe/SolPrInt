#ifndef MOTOR
#define MOTOR
#include <AccelStepper.h>

// Define the pins used for the stepper motor
#define IN1 14
#define IN2 27
#define IN3 26
#define IN4 25
#define TOTALANGLE 296.22
#define MAXSPEED 80

void initializeMotor(int previousSpeed);
void setMotortoPosition(int speedPress, int previousSpeed); 
void setMotortoIni(); 
float linearMap(float x, float in_min, float in_max, float out_min, float out_max);  
float getCalibratedAngle(float targetAngle);  

#endif
