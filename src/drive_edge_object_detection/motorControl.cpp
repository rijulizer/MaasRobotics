#include "motorControl.h"
#include "Energia.h"  

// Initialize PWM pins for motor speed
void motorInit() {
  // Set motor control pins as OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set Enable pins as OUTPUT (PWM control for speed)
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
}

// Drive motors forward
void driveForward(int speed) {
  // Left motors forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motors forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Set speed using PWM
  analogWrite(ENA, speed);  // Full speed for left motor
  analogWrite(ENB, speed);  // Full speed for right motor
}

// Stop the car
void stopCar() {
  // Stop all motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Set PWM to 0 (no speed)
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// Drive motors backward
void driveBackward(int speed) {
  // Left motors backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right motors backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Set speed using PWM
  analogWrite(ENA, speed);  // Full speed for left motor
  analogWrite(ENB, speed);  // Full speed for right motor
}

// Turn left (stop left motors, run right motors)
void turnLeft(int speed) {
  // Stop left motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  // Run right motors forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Set speed using PWM
  analogWrite(ENA, 0);   // Left motor stopped
  analogWrite(ENB, speed); // Full speed for right motor
}

// Turn right (stop right motors, run left motors)
void turnRight(int speed) {
  // Stop right motors
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Run left motors forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Set speed using PWM
  analogWrite(ENA, speed); // Full speed for left motor
  analogWrite(ENB, 0);   // Right motor stopped
}
