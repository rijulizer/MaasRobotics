#ifndef IMU_H  // Header guard to avoid multiple inclusions
#define IMU_H

#define SDA 10  // 10 - PA7
#define SCL 9 // 9 - PA6

#include "Arduino.h"

// Declare the IR sensor class
class IMU {
  private:
    const int MPU = 0x68; // MPU6050 I2C address
    float AccX, AccY, AccZ;
    float GyroX, GyroY, GyroZ;
    float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
    float accRange, gyroRange;
    float roll, pitch, yaw;
    float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
    float elapsedTime, currentTime, previousTime;
    int c = 0;

  public:
    // Constructor to initialize the IMU
    IMU(); 
    void initialize();
    String getSensorDataJson(float AccX, float AccY, float AccZ, 
                        float GyroX, float GyroY, float GyroZ, 
                        float roll, float pitch, float yaw);
    float* get_data();

    // Method to check if something is detected based on threshold
    void calculate_IMU_error();
};

#endif 
