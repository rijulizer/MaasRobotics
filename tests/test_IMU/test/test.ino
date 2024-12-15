#include "IMU.h"

static float* data;
IMU imu = IMU(); 

void setup() {
  Serial2.begin(9600);
  Serial.begin(9600); 
  imu.initialize();
  Serial.println("Starting IMU test");
  
}


void sendArray(float* IMU_data) {
  char output[100] = "["; // Increased buffer size
  int offset = 1; // Start after the opening bracket
  char floatStr[10];

  for (int i = 0; i < 9; i++) {
    dtostrf(IMU_data[i], 4, 2, floatStr);
    offset += snprintf(output + offset, sizeof(output) - offset, "%.2f", IMU_data[i]);
    
    if (i < 8) {
      offset += snprintf(output + offset, sizeof(output) - offset, ", ");
    }
  }

  snprintf(output + offset, sizeof(output) - offset, "]");
  Serial.println(output); // Send the entire formatted string
}


void loop() { 
  data = imu.get_data();
  //sendArray(data);
  
  Serial2.println("IMU data:");
  for (int i = 0; i < 9; i++) {
    Serial2.print(data[i]); 
    if (i < 8){
      Serial2.print(", ");}
  }
}
