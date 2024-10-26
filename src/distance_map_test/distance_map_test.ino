#include "distance_map_test.h"

const int trigPin = 36; // PC5 36
const int echoPin = 37; // PC4 37
MovingUSSensor USSensorTop(trigPin, echoPin);

// Define the maximum number of positions
const int MAX_ENTRIES = 179;

// Arrays to store dictionary keys (int) and values (int)
int values[MAX_ENTRIES];
int currentSize = 0;

void setup() {
  Serial.begin(9600);
}

// Function to initialize keys array to an invalid state
void InitializeValues() {
  currentSize=0;
  for (int i = 0; i < MAX_ENTRIES; i++) {
    values[i] = -1;
  }
}

void AddToValues(int key, int value) {
  // If key does not exist and there's space, add a new entry
  if (currentSize <= MAX_ENTRIES) {
    values[key]=value;
    Serial.print("Updated key ");
    Serial.print(key);
    Serial.print(" with value ");
    Serial.println(value);
    currentSize++;
  } else {
    Serial.println("Error: Map is full!");
  }
}

int* getReadingsAtAllPositions(){
  InitializeValues();
  int rotationPosition = 0;
  while(rotationPosition<180){
    //Function for setting motor position // Delay for the motor to set its position
    float valueAtPosition1 = USSensorTop.objectDetectedDistance(); // Read teh sensor value at that position
    delay(200);
    float valueAtPosition2 = USSensorTop.objectDetectedDistance(); // Read teh sensor value at that position
    delay(200);
    float valueAtPosition3 = USSensorTop.objectDetectedDistance(); // Read teh sensor value at that position
    delay(200);
    float valueAtPosition4 = USSensorTop.objectDetectedDistance(); // Read teh sensor value at that position
    delay(200);
    float valueAtPosition5 = USSensorTop.objectDetectedDistance(); // Read teh sensor value at that position
    // Add the value for the position in the map
    AddToValues(rotationPosition, ((int)((valueAtPosition1+valueAtPosition2+valueAtPosition3+valueAtPosition4+valueAtPosition5)/5))); 
    
    rotationPosition++;
  }

  return values;
}


void loop() {
  int* scannedValues = getReadingsAtAllPositions();
  Serial.println("Scan complete!");
  Serial.println("First value:");
  Serial.println(scannedValues[0]);
  Serial.println("Last value:");
  Serial.println(scannedValues[179]);
  delay(5);
}
