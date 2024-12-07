// Energia Sketch for EK-TM4C123GXL
void setup() {
  // Initialize UART2 on PD6 for receiving data
  Serial2.begin(9600); // UART2 (PD6 = RX) baud rate must match ESP32's Serial baud rate

  // Initialize Serial Monitor (connected via UART0)
  Serial.begin(9600);  // UART0 for USB Serial Monitor
  Serial.println("Ready to receive data from ESP32-CAM on PD6...");
}

void loop() {
  // Check if data is available on UART2
  if (Serial2.available() > 0) {
    String receivedData = "";

    // Read the incoming string until a newline character or timeout
    while (Serial2.available() > 0) {
      Serial.println(Serial2.readString()); // Read one character from UART2 
      delay(5);                // Small delay for stable data reception
    }
  }
}
