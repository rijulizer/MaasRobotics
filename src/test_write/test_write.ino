
void setup() {
  Serial2.begin(9600); // UART2 (PD6 = RX) baud rate must match ESP32's Serial baud rate

  // Initialize Serial Monitor (connected via UART0)
  Serial.begin(9600);  // UART0 for USB Serial Monitor
}

void loop() {

  Serial2.println("Hello from EK-TM4C123GXL");
  delay(30000);
}
