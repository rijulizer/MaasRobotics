#include "motorControl.h"
#include "IRSensor.h"

IRSensor IRSensorL(28, 140);
IRSensor IRSensorR(27, 140);

void setup() {

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
  if (IRSensorL.groundDetected() || IRSensorR.groundDetected()) {
    // Stop the car if the ground is not detected
    driveForward(100);
    delay(10);
  }else{
      driveBackward(100);
      delay(200);
      stopCar();
      delay(5000);
  }

}
