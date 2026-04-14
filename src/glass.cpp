#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"
#include "bitmaps.h" // Your 200+ hex arrays must be in this file

// OLED Hardware Config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Bluetooth Object
BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);

    // 1. Init Bluetooth
    if (!SerialBT.begin("ESP32_Sign_HUD")) {
        Serial.println("BT Init Failed");
    }

    // 2. Init OLED (SDA=21, SCL=22)
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); 
    }

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(20, 30);
    display.print("HUD ACTIVE");
    display.display();
}

void loop() {
    // 3. Listen for Voice Data from Phone
    if (SerialBT.available()) {
        String input = SerialBT.readString();
        input.trim();
        input.toLowerCase();

        display.clearDisplay();

        // 4. Logic for 10-200 signs
        if (input == "salam") {
            display.drawBitmap(32, 0, salam, 64, 64, 1);
        } 
        else if (input == "help" || input == "thank you") {
            display.drawBitmap(32, 0, help, 64, 64, 1);
        }
        else if (input == "nice") {
            display.drawBitmap(32, 0, s_nice, 64, 64, 1);
        }
        // Add more else-if blocks for your specific 10 signs
        
        else {
            // Fallback: If no sign is found, show the raw word
            // display.setTextSize(2);
            // display.setCursor(0, 20);
            // display.print(input);

            display.drawBitmap(32, 0, cant, 64, 64, 1);
        }

        display.display();
    }
}