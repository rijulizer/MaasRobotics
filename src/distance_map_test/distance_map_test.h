#ifndef DISTANCE_MAP_TEST_H  // Header guard to avoid multiple inclusions
#define DISTANCE_MAP_TEST_H

// Declare the US sensor class
class MovingUSSensor {
  private:
    int trig_pin;  // The Trig pin of the US sensor
	  int echo_pin;  // echo pin 

  public:
    MovingUSSensor(int trigPin, int echoPin);

    // Method to read sensor value
    int read();

    // Method to check the distance of an object detected by the sensor
    int objectDetectedDistance();
};

#endif //
