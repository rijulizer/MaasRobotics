#include "IMU.h"
#include "Energia.h"
#include "Wire.h"

/*
   Arduino and MPU6050 Accelerometer and Gyroscope Sensor Tutorial
   by Dejan, https://howtomechatronics.com

   Datasheet: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
*/

// Constructor
IMU::IMU() {
  
}

void IMU::initialize(){
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // Call this function if you need to get the IMU error values for your module
  calculate_IMU_error();
  delay(20);
}

float* IMU::get_data() {
  roll = 0.0;
  pitch = 0.0;
  yaw = 0.0;
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  accRange = 16384.0;//2g:16384.0  4g:8192,  8g:4096,  16g:2048
  AccX = (Wire.read() << 8 | Wire.read()) / accRange; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / accRange; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / accRange; // Z-axis value
  /*Serial.print("  AccX = ");Serial.print(AccX);
  Serial.print("  AccY = ");Serial.print(AccY);
  Serial.print("  AccZ = ");Serial.print(AccZ);*/
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY; // AccErrorY
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds.
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  gyroRange = 131.0; // 250:131.0,  500:65.5,  1000:32.8,  2000d/s:16.4
  GyroX = (Wire.read() << 8 | Wire.read()) / gyroRange; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / gyroRange;
  GyroZ = (Wire.read() << 8 | Wire.read()) / gyroRange;
  /*Serial.print("  GyroX = ");Serial.print(GyroX);
  Serial.print("  GyroY = ");Serial.print(GyroY);
  Serial.print("  GyroZ = ");Serial.print(GyroZ);
  Serial.print(" ");*/

  // Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX; // GyroErrorX 
  GyroY = GyroY - GyroErrorY; // GyroErrorY 
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ 
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  // Print the values on the serial monitor
  /*Serial.print(roll);
  Serial.print("/");
  Serial.print(pitch);
  Serial.print("/");
  Serial.println(yaw);*/
  // Assuming sensor data is already calculated
  static float IMU_data[9] = {AccX, AccY, AccZ, GyroX, GyroY, GyroZ, roll, pitch, yaw};
  //String jsonData = getSensorDataJson(AccX, AccY, AccZ, GyroX, GyroY, GyroZ, roll, pitch, yaw);
  return IMU_data;
}

String IMU::getSensorDataJson(float AccX, float AccY, float AccZ, 
                               float GyroX, float GyroY, float GyroZ, 
                               float roll, float pitch, float yaw) {
    Serial.print(AccX);
    Serial.print(", ");
    Serial.print(AccY);
    Serial.print(", ");
    Serial.print(AccZ);
    Serial.print(", ");
    Serial.print(GyroX);
    Serial.print(", ");
    Serial.print(GyroY);
    Serial.print(", ");
    Serial.print(GyroZ);
    Serial.print(", ");
    Serial.print(roll);
    Serial.print(", ");
    Serial.print(pitch);
    Serial.print(", ");
    Serial.println(yaw); 
    char temp[100];
    int lena = snprintf(temp, sizeof(temp), 
                       "AccX : %.1f", AccX);
    Serial.println(lena);
    Serial.println(temp);
    
    // Temporarily allocate a buffer to get the required length for snprintf
    char tempBuffer[256]; // Temporary buffer for snprintf
    
    // Create the JSON string in the temporary buffer to get the length
    int len = snprintf(tempBuffer, sizeof(tempBuffer), 
                       "{"
                       "\"AccX\":%.3f,"
                       "\"AccY\":%.3f,"
                       "\"AccZ\":%.3f,"
                       "\"GyroX\":%.3f,"
                       "\"GyroY\":%.3f,"
                       "\"GyroZ\":%.3f,"
                       "\"roll\":%.3f,"
                       "\"pitch\":%.3f,"
                       "\"yaw\":%.3f"
                       "}",
                       AccX, AccY, AccZ, 
                       GyroX, GyroY, GyroZ, 
                       roll, pitch, yaw);
    //Serial.println(len);
    if (len < 0) {
        return ""; // Error in snprintf
    }
    //Serial.println(tempBuffer);
    // Create the JSON string directly using String class
    String jsonStr = String(tempBuffer); // Create a String from the temporary buffer
   // Serial.println(jsonStr);
    return jsonStr; // Return the JSON string
}


void IMU::calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  /*// Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);*/
}
