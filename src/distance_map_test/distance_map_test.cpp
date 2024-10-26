#include "Energia.h"
#include "distance_map_test.h"

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

// Constructor to initialize pin and optional threshold
MovingUSSensor::MovingUSSensor(int trigPin, int echoPin) {
  trig_pin = trigPin;  // Set the pins
  echo_pin = echoPin;
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

// Method to read the sensor value
int MovingUSSensor::read() {
	// establish variables for duration of the ping, 
	// and the distance result in inches and centimeters:
	long duration, inches, cm;

	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	digitalWrite(trig_pin, LOW);
	delayMicroseconds(2);
	// Giving the trigger
	digitalWrite(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trig_pin, LOW);

	// read the echo pin
	duration = pulseIn(echo_pin, HIGH);

	// convert the time into a distance
	//inches = microsecondsToInches(duration);
	cm = microsecondsToCentimeters(duration);
	return cm;  // Return the analog value from the sensor
}

// Method to get the sensor reading
int MovingUSSensor::objectDetectedDistance() {
  int sensorValue = read();
  return sensorValue;
}
