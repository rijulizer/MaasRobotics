#include "motorControl.h"
#include "IRSensor.h"
#include "USSensor.h"
#include "Servo.h"

// Create the servo object
Servo myservo;
int servo_pin = 23; // PD0

// Instantiate US sensors
const int trigPin = 11; //PA2 11
const int echoPin = 31; //PF4 31 
const long us_threshold = 20; 
const long ir_thresholdLeft = 200;
const long ir_thresholdRight = 400;
const unsigned long reverseDuration = 20;
const unsigned long turnDuration = 450;
USSensor USSensorF(trigPin, echoPin, us_threshold);

// Instantiate IR sensors (using only left IR sensor since right one is broken)
IRSensor IRSensorL(28, ir_thresholdLeft); // PE2 28
IRSensor IRSensorR(26, ir_thresholdRight); //PE3 26

unsigned long previousMillis = 0;
unsigned long actionStartTime = 0;
const long interval = 100;  // Time between movement updates in milliseconds

enum RobotState { MOVING_FORWARD, REVERSING, STOPPED };
RobotState currentState = MOVING_FORWARD;

void setup() {
  Serial.begin(9600); 
  myservo.attach(servo_pin); // attach the servo to our servo object
  myservo.write(90);
  motorInit();  // Initialize motor control pins
  driveForward(100);  // Start moving forward initially
  Serial.println("Robot initialized, moving forward.");
}

void loop() {
  //myservo.write(50);
  //delay(1000);
  //myservo.write(70);
  //delay(1000);
  //myservo.write(60);
  
  unsigned long currentMillis = millis();  // Get the current time
  // Check sensors
  bool isEdgeDetectedLeft = IRSensorL.edgeDetected();
  bool isEdgeDetectedRight = IRSensorR.edgeDetected();
  bool isObjectDetected = USSensorF.objectDetected(); // Ultrasonic front sensor
  switch (currentState) {
    case MOVING_FORWARD:
      if (isEdgeDetectedLeft || isEdgeDetectedRight || isObjectDetected) {
        // Edge or object detected, start reversing
        Serial.println("Object/Edge Detected, reversing!");
        Serial.println("R");
        Serial.println(isEdgeDetectedRight);
        Serial.println("L");
        Serial.println(isEdgeDetectedLeft);
        Serial.println("USS");
        Serial.println(isObjectDetected);
        currentState = REVERSING;
        actionStartTime = currentMillis;
        driveBackward(255);  // Reverse at full speed
      } else {
        // Keep moving forward smoothly
        if (currentMillis - previousMillis >= interval) {
          // Update the previous time stamp
          previousMillis = currentMillis;
          driveForward(110);
          driveForward(70);  // Keep moving forward at speed x
          Serial.println("Clean road ahead, moving forward.");
        }
      }
      break;

    case REVERSING:
      // Keep reversing until the reverse duration has passed
      if (currentMillis - actionStartTime >= reverseDuration) {
        // After reversing for the set duration, stop the car
        stopCar();
        Serial.println("Finished reversing, stopping the car.");
        delay(100);  // Small pause to ensure full stop
        currentState = STOPPED;  // Transition to stopped state
      }
      break;

    case STOPPED:
      // After stopping, resume moving forward if no obstacle is in the way
      if (isEdgeDetectedLeft || isEdgeDetectedRight) {
        Serial.println("Right detected");
        Serial.println(isEdgeDetectedRight);
        // Edge or object detected, start reversing
        driveBackward(120);
        delay(500);
        if(isEdgeDetectedLeft){
          turnRight(180);
        }else{
          turnLeft(180);  
        }
        delay(turnDuration);
      }else if (isObjectDetected) {
        Serial.println("USS detected object!");
        // Edge or object detected, start reversing
        driveBackward(120);
        delay(340);
        turnRight(190);
        delay(turnDuration*2);
      }else{
        Serial.println("Resuming forward movement.");
        driveForward(100);  // Resume forward movement
        currentState = MOVING_FORWARD; 
      }
      break;
  }
}
