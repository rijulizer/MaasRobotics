#include "IRSensor.h"

IRSensor IRSensorL(28, 140);
IRSensor IRSensorR(26, 140);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly: 
  int IRvalueL = IRSensorL.read();
  int IRvalueR = IRSensorR.read();

  Serial.print("Analog Left Reading: ");
  Serial.print(IRvalueL);   // Print the left sensor value
  Serial.print(" | ");      // Print a separator
  Serial.print("Analog Right Reading: ");
  Serial.println(IRvalueR);
}
