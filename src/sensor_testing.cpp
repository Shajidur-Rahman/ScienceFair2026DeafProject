#include <Arduino.h>

// A1302 Hall Effect Sensor Testing
// This code tests the A1302 sensor to determine if it's in good condition

// Pin configuration
const int SENSOR_PIN = A0;  // Analog pin for sensor input
const int LED_GOOD = 13;    // LED for good sensor status
const int LED_BAD = 12;     // LED for bad sensor status

// Sensor parameters
const float VREF = 5.0;     // Reference voltage (5V for Arduino)
const int ADC_RESOLUTION = 1023;  // 10-bit ADC

// Testing thresholds
const int NUM_SAMPLES = 100;  // Number of samples for averaging
const float MIN_OUTPUT = 1.5; // Minimum expected output voltage (V)
const float MAX_OUTPUT = 3.5; // Maximum expected output voltage (V)
const float STABILITY_THRESHOLD = 0.1; // Voltage variation threshold

void setup() {
  Serial.begin(9600);
  
  pinMode(SENSOR_PIN, INPUT);
  pinMode(LED_GOOD, OUTPUT);
  pinMode(LED_BAD, OUTPUT);
  
  digitalWrite(LED_GOOD, LOW);
  digitalWrite(LED_BAD, LOW);
  
  Serial.println("========================================");
  Serial.println("A1302 Hall Effect Sensor Diagnostic Test");
  Serial.println("========================================");
  Serial.println("");
}

void loop() {
  Serial.println("Starting sensor test...");
  Serial.println("");
  
  // Read raw sensor values
  float voltage = readSensorVoltage();
  float stability = testSensorStability();
  float rangeTest = testVoltageRange(voltage);
  
  // Display results
  displayTestResults(voltage, stability, rangeTest);
  
  // Determine sensor health
  bool isSensorGood = evaluateSensorHealth(voltage, stability);
  
  // Update status LEDs
  if (isSensorGood) {
    digitalWrite(LED_GOOD, HIGH);
    digitalWrite(LED_BAD, LOW);
    Serial.println("✓ SENSOR STATUS: GOOD");
  } else {
    digitalWrite(LED_GOOD, LOW);
    digitalWrite(LED_BAD, HIGH);
    Serial.println("✗ SENSOR STATUS: BAD - Check connections or replace sensor");
  }
  
  Serial.println("");
  Serial.println("----------------------------------------");
  Serial.println("Test will repeat in 5 seconds...");
  Serial.println("----------------------------------------");
  Serial.println("");
  
  delay(5000);  // Wait 5 seconds before next test
}

// Function to read average sensor voltage
float readSensorVoltage() {
  float totalVoltage = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int rawValue = analogRead(SENSOR_PIN);
    float voltage = (rawValue / (float)ADC_RESOLUTION) * VREF;
    totalVoltage += voltage;
    delay(10);
  }
  
  return totalVoltage / NUM_SAMPLES;
}

// Function to test sensor stability
float testSensorStability() {
  float minVoltage = 5.0;
  float maxVoltage = 0.0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int rawValue = analogRead(SENSOR_PIN);
    float voltage = (rawValue / (float)ADC_RESOLUTION) * VREF;
    
    if (voltage < minVoltage) minVoltage = voltage;
    if (voltage > maxVoltage) maxVoltage = voltage;
    
    delay(10);
  }
  
  return maxVoltage - minVoltage;
}

// Function to test if voltage is in valid range
float testVoltageRange(float voltage) {
  if (voltage >= MIN_OUTPUT && voltage <= MAX_OUTPUT) {
    return 1.0;  // 100% - In range
  } else if (voltage < MIN_OUTPUT) {
    return (voltage / MIN_OUTPUT) * 100;
  } else {
    return ((MAX_OUTPUT / voltage) * 100);
  }
}

// Function to display test results
void displayTestResults(float voltage, float stability, float rangePercent) {
  Serial.print("Current Voltage Output: ");
  Serial.print(voltage, 3);
  Serial.println(" V");
  
  Serial.print("Valid Range: ");
  Serial.print(MIN_OUTPUT, 1);
  Serial.print("V - ");
  Serial.print(MAX_OUTPUT, 1);
  Serial.println("V");
  
  Serial.print("Voltage Stability (variation): ");
  Serial.print(stability, 3);
  Serial.println(" V");
  
  Serial.print("Stability Status: ");
  if (stability <= STABILITY_THRESHOLD) {
    Serial.println("✓ Stable");
  } else {
    Serial.println("✗ Unstable");
  }
  
  Serial.print("Range Test Result: ");
  if (rangePercent >= 95) {
    Serial.print("✓ PASS (");
  } else {
    Serial.print("✗ FAIL (");
  }
  Serial.print(rangePercent, 1);
  Serial.println("%)");
  Serial.println("");
}

// Function to evaluate overall sensor health
bool evaluateSensorHealth(float voltage, float stability) {
  bool voltageOK = (voltage >= MIN_OUTPUT && voltage <= MAX_OUTPUT);
  bool stabilityOK = (stability <= STABILITY_THRESHOLD);
  
  return voltageOK && stabilityOK;
}
