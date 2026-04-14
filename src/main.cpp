#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <final_deaf_inferencing.h> 

#define I2C_SDA 8 
#define I2C_SCL 9

// UART Pins for Arduino Communication
#define TX1_PIN 43 
#define RX1_PIN 44

// --- VOICE CYCLE VARIABLE ---
int VOICE_CYCLE_THRESHOLD = 2; // Change this number to control how often it speaks
int stableSignCount = 0; 
String lastLabel = "";
int detectionCounter = 0;

Adafruit_MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);

    while (!Serial); 
    delay(2000); 
    Serial.println("--- GLOVE AI: BOOTING ---");

    Wire.begin(I2C_SDA, I2C_SCL);

    // Your original MPU 0x70 fix
    if (!mpu.begin(0x68, &Wire)) { 
        Serial.println("Warning: Library reported failure, but continuing...");
    }

    Serial.println("MPU WOKEN UP!");

    // Your original MPU settings
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    for(int i=1; i<=5; i++) {
        pinMode(i, INPUT);
    }
    Serial.println("SYSTEM READY. START SIGNING.");
}

void loop() {
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

    // --- YOUR ORIGINAL AI DATA COLLECTION ---
    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 11) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        buffer[ix + 0] = a.acceleration.x;
        buffer[ix + 1] = a.acceleration.y;
        buffer[ix + 2] = a.acceleration.z;
        buffer[ix + 3] = g.gyro.x;
        buffer[ix + 4] = g.gyro.y;
        buffer[ix + 5] = g.gyro.z;
        
        buffer[ix + 6] = (float)analogRead(1);
        buffer[ix + 7] = (float)analogRead(2);
        buffer[ix + 8] = (float)analogRead(3);
        buffer[ix + 9] = (float)analogRead(4);
        buffer[ix + 10] = (float)analogRead(5);

        while (micros() < next_tick);
    }

    signal_t signal;
    numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    ei_impulse_result_t result = { 0 };
    if (run_classifier(&signal, &result, false) == EI_IMPULSE_OK) {
        bool highConfidenceFound = false;

        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (result.classification[ix].value > 0.85) {
                String currentLabel = result.classification[ix].label;
                highConfidenceFound = true;

                // ALWAYS print to serial - never stops
                Serial.printf("SIGN: %s (%d%%)\n", 
                              currentLabel.c_str(), 
                              (int)(result.classification[ix].value * 100));

                // --- VOICE CYCLE LOGIC ---
                if (currentLabel == lastLabel) {
                    detectionCounter++;
                } else {
                    detectionCounter = 1; 
                    lastLabel = currentLabel;
                }

                // If the sign is "stable" for 2 frames, count it as one "arrival"
                if (detectionCounter >= 2) {
                    stableSignCount++;
                    detectionCounter = 0; 

                    // Trigger Arduino only when cycles hit your threshold
                    if (stableSignCount >= VOICE_CYCLE_THRESHOLD) {
                        if (currentLabel == "Help") Serial1.print('H');
                        else if (currentLabel == "Salam") Serial1.print('S');
                        else if (currentLabel == "Water") Serial1.print('W');

                        stableSignCount = 0; // Reset cycle
                    }
                }
            }
        }
        if (!highConfidenceFound) {
            detectionCounter = 0;
            lastLabel = "";
        }
    }
}