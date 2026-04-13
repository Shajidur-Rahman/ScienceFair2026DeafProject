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

// --- CALIBRATION: Use your recorded raw values ---
// If 'Open' is higher than 'Closed', the math still works!
int tMin = 2130, tMax = 2125; 
int iMin = 2085, iMax = 2250;
int mMin = 2040, mMax = 1845;
int rMin = 2010, rMax = 1845;
int pMin = 1945, pMax = 1750;

float normalize(int raw, int minVal, int maxVal) {
  if (minVal == maxVal) return 0.0;
  float n = (float)(raw - minVal) / (float)(maxVal - minVal);
  return constrain(n, 0.0, 1.0);
}

void setup() {
  // 1. Set to the stable speed
  Serial.begin(115200); 
  delay(2000); 

  analogReadResolution(12);
  Wire.begin(SDA_PIN, SCL_PIN, 100000); // Standard I2C speed for stability
  delay(100);

  // 2. MPU6050 Hard Reset & Wakeup
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0x80); // Reset command
  Wire.endTransmission();
  delay(100);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0x00); // Wake up command
  if (Wire.endTransmission() != 0) {
    // If this fails, the forwarder will see this error
    while(1) { Serial.println("ERR: MPU_NOT_FOUND"); delay(1000); }
  }
}

void loop() {
  // 3. Get MPU Data
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  float ax = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  float ay = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  float az = (int16_t)(Wire.read()<<8 | Wire.read()) / 16384.0;
  Wire.read()<<8 | Wire.read(); 
  float gx = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;
  float gy = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;
  float gz = (int16_t)(Wire.read()<<8 | Wire.read()) / 131.0;

  // 4. Get Normalized Finger Data
  float t = normalize(analogRead(thumbPin),  tMin, tMax);
  float i = normalize(analogRead(indexPin),  iMin, iMax);
  float m = normalize(analogRead(middlePin), mMin, mMax);
  float r = normalize(analogRead(ringPin),   rMin, rMax);
  float p = normalize(analogRead(pinkyPin),  pMin, pMax);

  // 5. Print CSV ONLY (No text headers)
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

  delay(20); // 50Hz
}