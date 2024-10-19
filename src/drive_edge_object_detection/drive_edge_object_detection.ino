#include "motorControl.h"
#include "IRSensor.h"
#include "USSensor.h"

// Instantiate US sensors
const int trigPin = 36; //PC5 36
const int echoPin = 37; //PC4 37 
const long us_threshold = 20; 
const long ir_threshold = 200;
const unsigned long reverseDuration = 20;
const unsigned long turnDuration = 450;
USSensor USSensorF(trigPin, echoPin, us_threshold);

// Instantiate IR sensors (using only left IR sensor since right one is broken)
IRSensor IRSensorL(28, ir_threshold); // PE2 28

unsigned long previousMillis = 0;
unsigned long actionStartTime = 0;
const long interval = 100;  // Time between movement updates in milliseconds

enum RobotState { MOVING_FORWARD, REVERSING, STOPPED };
RobotState currentState = MOVING_FORWARD;

void setup() {
  Serial.begin(9600); 
  motorInit();  // Initialize motor control pins
  driveForward(100);  // Start moving forward initially
  Serial.println("Robot initialized, moving forward.");
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time
  
  // Check sensors
  bool isEdgeDetected = IRSensorL.edgeDetected(); // Only use the left IR sensor
  bool isObjectDetected = USSensorF.objectDetected(); // Ultrasonic front sensor
  
  switch (currentState) {
    case MOVING_FORWARD:
      if (isEdgeDetected || isObjectDetected) {
        Serial.println("IR left sensor: ");
        Serial.println(isEdgeDetected);
        Serial.println("US sensor: ");
        Serial.println(isObjectDetected);
        // Edge or object detected, start reversing
        Serial.println("Object/Edge Detected, reversing!");
        currentState = REVERSING;
        actionStartTime = currentMillis;
        driveBackward(255);  // Reverse at full speed
      } else {
        // Keep moving forward smoothly
        if (currentMillis - previousMillis >= interval) {
          // Update the previous time stamp
          previousMillis = currentMillis;
          driveForward(100);  // Keep moving forward at speed 100
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
      if (isEdgeDetected) {
        // Edge or object detected, start reversing
        driveBackward(120);
        delay(500);
        turnRight(180);
        delay(turnDuration);
      }else if (isObjectDetected) {
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
