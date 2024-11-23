int incomingByte = 0; // for incoming serial data

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  //Serial.write(45); // send a byte with the value 45

  //int bytesSent = Serial.write("hello\n");  //send the string “hello” and return the length of the string.
  // send data only when you receive data:
  //Serial.println(Serial.available());
  // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.println("I received: ");
    Serial.println(incomingByte, DEC);
  /*if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.println("I received: ");
    Serial.println();
    Serial.println(incomingByte, DEC);
  }*/
}
