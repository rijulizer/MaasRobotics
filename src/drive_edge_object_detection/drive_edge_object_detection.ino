#include "motorControl.h"
#include "IRSensor.h"
#include "USSensor.h"
#include "Servo.h"
//#include "IMU.h"


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

// Create the IMU object
/* IMU imu = IMU(); 
char* IMU_data;
float* data;*/

unsigned long previousMillis = 0;
unsigned long actionStartTime = 0;
const long interval = 100;  // Time between movement updates in milliseconds

enum RobotState { MOVING_FORWARD, REVERSING, STOPPED, WAITING_INSTRUCTIONS };
RobotState currentState = MOVING_FORWARD;

void setup() {
  Serial2.begin(9600); // UART2 (PD6 = RX, PD7 = TX) baud rate must match ESP32's Serial baud rate
  Serial.begin(9600); 
  myservo.attach(servo_pin); // attach the servo to our servo object
  myservo.write(90);
//  imu.initialize();
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
 /* if currentState != WAITING_INSTRUCTIONS;{
    data = imu.get_data();
    for (int i = 0; i < 9; i++) {
      Serial2.println(data[i]); 
    }
    //Serial2.println(IMU_data);
  }*/

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
        //currentState = WAITING_INSTRUCTIONS; 
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
      }
      break;

    case WAITING_INSTRUCTIONS:
      Serial.println("Waiting for instructions");
      Serial2.println("EK-TM4C123GXL stopped and waiting for instructions");
      delay(2000); // Wait for data to be send before receiving

       // Check if data is available on UART2
      if (Serial2.available() > 0) {
        String receivedData = "";

        Serial.println("Received instructions: ");
        // Read the incoming string until a newline character or timeout
        while (Serial2.available() > 0) {
          Serial.println(Serial2.readString()); // Read one character from UART2 
          delay(5);                // Small delay for stable data reception
        }
      }else{
        delay(500); // Don't read constantly
      }  
      break;

  }
}
