#ifndef MOTORCONTROL_H  // Header guard to avoid multiple inclusions
#define MOTORCONTROL_H


// Motor control pins
#define IN1    6  // Left Motors - PE5
#define IN2    5  // Left Motors - PE4
#define IN3    4  // Right Motors - PB1
#define IN4    3  // Right Motors - PB0

// Enable pins (PWM pins for speed control)
#define ENA    7  // Left Motors - PB4
#define ENB    2  // Right Motors - PB5


// Declare the motor control functions
void motorInit();
void driveForward(int speed);
void turnRight(int speed);
void stopCar();
void driveBackward(int speed);
void turnLeft(int speed);

#endif
