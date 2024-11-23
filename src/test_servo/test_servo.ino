// Include the library
#include "Servo.h"

// Create the servo object
Servo myservo;
int pinNb = 23; // PD0

// Setup section to run once
void setup() {
  myservo.attach(pinNb); // attach the servo to our servo object
  myservo.write(90); // stop the motor
}

// Loop to keep the motor turning!
void loop() {
  //Serial.print(analogRead(pinNb));
  
  //myservo.write(0); // rotate the motor clockwise
  //delay(2000); // keep rotating for 1 seconds (1000 milliseconds)

  myservo.write(80); // rotate the motor counter-clockwise
  delay(2000); //

  myservo.write(100); // rotate the motor counter-clockwise
  delay(2000); //

  //myservo.write(180); // rotate the motor counter-clockwise
  //delay(2000); //
}
