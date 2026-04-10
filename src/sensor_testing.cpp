#include <Arduino.h>

// A1302 Hall Effect Sensor - Simple Value Display
const int SENSOR_PIN = A0;
long readingNumber = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  Serial.println("A1302 Sensor Values:");
}

void loop() {
  readingNumber++;
  
  // Read raw ADC value
  int rawValue = analogRead(SENSOR_PIN);
  
  // Convert to voltage (0-4095 ADC for ESP32)
  float voltage = (rawValue / 4095.0) * 3.3;
  
  // Display values
  Serial.print("NUM: ");
  Serial.println(readingNumber);
  Serial.print("RAW: ");
  Serial.print(rawValue);
  Serial.print(" | VOLTAGE: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  Serial.println("---");
  
  delay(100);  // 100ms delay for continuous display
}
