#include "motorControl.h"
#include "IRSensor.h"
#include "USSensor.h"

// Instantiate US sensors
const int trigPin = 36; //PC5 36
const int echoPin = 37; //PC4 37 
const long us_threshold = 10; 
USSensor USSensorF(trigPin, echoPin, us_threshold);

// Instantiate IR sensors
IRSensor IRSensorL(28, 140); // PE2 28
IRSensor IRSensorR(27, 140); // PE1 27 

void setup() {
  Serial.begin(9600); 
  motorInit();  // Initialize motor control pins
}

//void loop() {
//  // Move forward for 5 seconds
//  driveForward();
//  delay(5000);  // Move forward for 5 seconds
//
//  // Turn right
//  turnRight();
//  delay(5000);  // Turn right for 5 seconds
//
//  // Move forward again
//  driveForward();
//  delay(5000);  // Move forward for 5 seconds
//
//  // Stop the car
//  stopCar();
//  delay(1000);  // Wait for 1 second before next movement
//}


void loop(){
  
  if (IRSensorL.edgeDetected() || IRSensorR.edgeDetected() || USSensorF.objectDetected()) {
    Serial.print("Object/Edge Detected");
    // Stop the car if edge or object is detected
//      driveBackward(200);
//      delay(20);
      stopCar();
      delay(10);
  }
  else{
    Serial.print("Clean Road!!");
    driveForward(100);
    delay(100);
  }

}
