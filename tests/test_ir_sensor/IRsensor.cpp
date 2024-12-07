#include "Energia.h"
#include "IRSensor.h"

// Constructor to initialize pin and optional threshold
IRSensor::IRSensor(int sensorPin, int thresh) {
  pin = sensorPin;  // Set the pin
  threshold = thresh;  // Set the threshold value (can be adjusted based on sensor)
  pinMode(pin, INPUT);  // Set the pin mode to INPUT
}

// Method to read the sensor value
int IRSensor::read() {
  return analogRead(pin);  // Return the analog value from the sensor
}

// Method to check if the sensor detects an object
bool IRSensor::groundDetected() {
  int sensorValue = read();  // Read the sensor value
  return sensorValue < threshold;  // If above the threshold, consider it detected
}
