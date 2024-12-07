/* HC_SRO4 Ultra Sonic Sensor
  
   This sketch reads a PING))) ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse 
   to return.  The length of the returning pulse is proportional to 
   the distance of the object from the sensor.    
 */

#ifndef USSENSOR_H  // Header guard to avoid multiple inclusions
#define USSENSOR_H

// Declare the IR sensor class
class USSensor {
  private:
    int trig_pin;  // The Trig pin of the US sensor
	  int echo_pin;  // echo pin 
    int threshold;  // Threshold value for detecting an object (if using analog sensor)

  public:
    // Constructor to initialize the IR sensor
    USSensor(int trigPin, int echoPin, int thresh = 4);  // Default threshold of 4 (can be adjusted)

    // Method to read sensor value
    int read();

    // Method to check if something is detected based on threshold
    bool objectDetected();
};

#endif //
