#ifndef IRSENSOR_H  // Header guard to avoid multiple inclusions
#define IRSENSOR_H


// Motor control pins
#define LEFTIR    6  // Left IR Sensor - PE5
#define RIGHTIR   5  // Left IR Sensor - PE4

// Declare the IR sensor class
class IRSensor {
  private:
    int pin;  // The pin to which the IR sensor is connected
    int threshold;  // Threshold value for detecting an object (if using analog sensor)

  public:
    // Constructor to initialize the IR sensor
    IRSensor(int sensorPin, int thresh = 512);  // Default threshold of 512 (can be adjusted)

    // Method to read sensor value
    int read();

    // Method to check if something is detected based on threshold
    bool groundDetected();
};

#endif // IRSENSOR_H
