#include <HardwareSerial.h>
#include <Arduino.h>

// Use UART2: RX = GPIO 16, TX = GPIO 17
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  
  Serial.println("ESP32 Started. Sending PLAY command in 5 seconds...");
  delay(5000);
  
  // Send command to Arduino
  Serial2.println("PLAY_SONG");
}

void loop() {
  // You can add WiFi or Bluetooth code here to trigger the command
}