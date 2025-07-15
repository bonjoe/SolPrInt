#include "motor.h"

// Create an instance of AccelStepper with the ULN2003
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
int targetPosition = 0;
float targetAngle = 0;
int pos = 0;
int delayMotor = 50;

// Target angles (expected) and measured angles (actual results from testing)
float targetAngles[] =   {0, 74,    96,     118,    140,    166,    188,    210,    233,    259}; 
float measuredAngles[] = {0, 83.13, 103.86, 126.14, 148.05, 168.42, 190.44, 210.99, 231.80, 250.61};  

void initializeMotor(int previousSpeed) {
  // Set maximum speed and acceleration (adjust as needed)
  stepper.setMaxSpeed(500.0);      // Maximum steps per second
  stepper.setAcceleration(delayMotor);   // Steps per second per second
  float rawAngle = map(previousSpeed, 0, MAXSPEED, 0, TOTALANGLE);
  targetAngle = getCalibratedAngle(rawAngle);
  targetPosition = map(targetAngle, 0, 720, 0, 4096);
  targetPosition = -targetPosition;
  stepper.setCurrentPosition(targetPosition);
  Serial.print("Set position to : ");  Serial.print(previousSpeed);Serial.print(" targetPosition : ");  Serial.println(targetPosition);  
}

void setMotortoPosition(int speedPress, int previousSpeed) {      
   // Map speed (0-80 RPM) to angle (0-296.22 degrees) for current position
   float rawAngle = map(speedPress, 0, MAXSPEED, 0, TOTALANGLE);
   targetAngle = getCalibratedAngle(rawAngle);
   // Convert angle to steps (4096 steps for 720 degrees)
   targetPosition = map(targetAngle, 0, 720, 0, 4096);
   // Reverse target position to achieve clockwise rotation
   targetPosition = -targetPosition;
   stepper.moveTo(targetPosition);
            
    while (stepper.distanceToGo() != 0) { stepper.run();yield();}
    
    // Print the speed and position
    Serial.print("Speed from : ");  Serial.print(previousSpeed);Serial.print(" To : "); Serial.print(speedPress);
    Serial.print("     target Angle : ");  Serial.println(targetAngle);
    delay(delayMotor); 
}

void setMotortoIni() {
    int lastPosition = 0;int stableCount = 0;
    // Keep moving back until it stops moving
    while (true) {
        stepper.moveTo(stepper.currentPosition() + 10);  // Small backward step
        stepper.run();
        delay(10);  // Small delay to let movement settle
        Serial.print("currentPosition : ");  Serial.println(stepper.currentPosition());
        if (stepper.currentPosition() == lastPosition) {
            stableCount++;
        } else {
            stableCount = 0;  // Reset count if it's still moving
        }
        lastPosition = stepper.currentPosition();
        // If the stepper hasn’t moved for 10 iterations (~1 second), assume it's at the limit
        if (stableCount > 10) {
            stepper.setCurrentPosition(0);
            break;
        }
        if (stepper.currentPosition() > 50) {
            stepper.setCurrentPosition(0);
            break;
        }
    }
    stepper.setCurrentPosition(0);
    Serial.println("Stepper reset to position 0");
    delay(delayMotor);
}

float linearMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);
}

float getCalibratedAngle(float targetAngle) {
  int i;
  for (i = 0; i < 9; i++) {  
    if (targetAngles[i] <= targetAngle && targetAngles[i + 1] >= targetAngle) {
      // Linear interpolation between measured values
      float corrected = measuredAngles[i] + 
         ((targetAngle - targetAngles[i]) / (targetAngles[i + 1] - targetAngles[i])) * 
         (measuredAngles[i + 1] - measuredAngles[i]);
      return corrected;
    }
  }
  return targetAngle;  // Default to input if out of range
}
