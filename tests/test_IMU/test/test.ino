#include "IMU.h"

IMU imu = IMU();

void setup() {
  Serial.begin(9600); 
}

void loop() {
  imu.get_data();

}
