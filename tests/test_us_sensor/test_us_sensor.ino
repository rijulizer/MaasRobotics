#include "USSensor.h"

const int trigPin = 36; //PC5 36
const int echoPin = 37; //PC4 37 
const long threshold = 10; 
// instantiate the US sesnor object 
USSensor USSensorF(trigPin, echoPin, threshold);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // read the distance value in cm
  int us_value = USSensorF.read();
  bool object_detected = USSensorF.objectDetected();
  
  // print serial output
  Serial.print("US sensor distance (cm): ");
  Serial.print(us_value);   // Print the left sensor value
  Serial.print(" | ");      // Print a separator
  Serial.print("Object Detected: ");
  Serial.println(object_detected);
}
