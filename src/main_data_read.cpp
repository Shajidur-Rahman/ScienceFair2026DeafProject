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

void setup() {
  Serial.begin(115200);
  
  // Start I2C manually
  Wire.begin(SDA_PIN, SCL_PIN, 100000); 
  delay(500);

  // Wake up MPU6050 manually (Power Management Register)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0);    
  if (Wire.endTransmission() != 0) {
    Serial.println("CRITICAL: MPU6050 Not Responding on 8/9!");
    while(1);
  }
  
  Serial.println("ax,ay,az,gx,gy,gz,thumb,index,middle,ring,pinky");
}

void loop() {
  // 1. Get Raw MPU Data
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); 
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14);

  // Read Accelerometer (High + Low bytes)
  int16_t ax = Wire.read()<<8 | Wire.read();
  int16_t ay = Wire.read()<<8 | Wire.read();
  int16_t az = Wire.read()<<8 | Wire.read();
  Wire.read()<<8 | Wire.read(); // Skip Temperature
  int16_t gx = Wire.read()<<8 | Wire.read();
  int16_t gy = Wire.read()<<8 | Wire.read();
  int16_t gz = Wire.read()<<8 | Wire.read();

  // 2. Get Finger Data
  int t = analogRead(thumbPin);
  int i = analogRead(indexPin);
  int m = analogRead(middlePin);
  int r = analogRead(ringPin);
  int p = analogRead(pinkyPin);

  // 3. Print in CSV format for Edge Impulse
  Serial.print(ax); Serial.print(",");
  Serial.print(ay); Serial.print(",");
  Serial.print(az); Serial.print(",");
  Serial.print(gx); Serial.print(",");
  Serial.print(gy); Serial.print(",");
  Serial.print(gz); Serial.print(",");
  Serial.print(t);  Serial.print(",");
  Serial.print(i);  Serial.print(",");
  Serial.print(m);  Serial.print(",");
  Serial.print(r);  Serial.print(",");
  Serial.println(p);

  delay(20); // 50Hz Sampling
}