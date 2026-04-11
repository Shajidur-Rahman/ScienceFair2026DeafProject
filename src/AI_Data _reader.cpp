#include <Arduino.h>
#include <Wire.h>

// Pins
const uint8_t thumbPin  = 1;
const uint8_t indexPin  = 2;
const uint8_t middlePin = 3;
const uint8_t ringPin   = 4;
const uint8_t pinkyPin  = 5;
#define SDA_PIN 8
#define SCL_PIN 9
#define MPU_ADDR 0x68

// --- CALIBRATION: Replace these with your actual raw values ---
int tMin = 1800, tMax = 3200;
int iMin = 1750, iMax = 3100;
int mMin = 1900, mMax = 3300;
int rMin = 1850, rMax = 3250;
int pMin = 1700, pMax = 3000;

// Helper function for clean normalization
float normalize(int raw, int minVal, int maxVal) {
  float n = (float)(raw - minVal) / (float)(maxVal - minVal);
  return constrain(n, 0.0, 1.0);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Ensure 0-4095 range for ESP32-S3
  
  Wire.begin(SDA_PIN, SCL_PIN, 400000); // 400kHz for faster I2C
  delay(500);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0);    
  if (Wire.endTransmission() != 0) {
    Serial.println("CRITICAL: MPU6050 Not Responding!");
    while(1);
  }
  
  // CSV Header
  Serial.println("ax,ay,az,gx,gy,gz,thumb,index,middle,ring,pinky");
}

void loop() {
  // 1. Get MPU Data and Normalize to Physical Units
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  // Scaled to G-force (+/- 2g) and Degrees/sec (+/- 250deg/s)
  float ax = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  float ay = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  float az = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  Wire.read()<<8 | Wire.read(); // Skip Temperature
  float gx = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;
  float gy = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;
  float gz = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;

  // 2. Get and Normalize Finger Data (0.0 to 1.0)
  float t = normalize(analogRead(thumbPin),  tMin, tMax);
  float i = normalize(analogRead(indexPin),  iMin, iMax);
  float m = normalize(analogRead(middlePin), mMin, mMax);
  float r = normalize(analogRead(ringPin),   rMin, rMax);
  float p = normalize(analogRead(pinkyPin),  pMin, pMax);

  // 3. Print Output
  Serial.print(ax, 3); Serial.print(",");
  Serial.print(ay, 3); Serial.print(",");
  Serial.print(az, 3); Serial.print(",");
  Serial.print(gx, 2); Serial.print(",");
  Serial.print(gy, 2); Serial.print(",");
  Serial.print(gz, 2); Serial.print(",");
  Serial.print(t, 3);  Serial.print(",");
  Serial.print(i, 3);  Serial.print(",");
  Serial.print(m, 3);  Serial.print(",");
  Serial.print(r, 3);  Serial.print(",");
  Serial.println(p, 3);

  delay(20); // Maintain 50Hz
}