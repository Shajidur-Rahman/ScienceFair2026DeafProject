#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <final_deaf_inferencing.h> 

// I2C
#define I2C_SDA 8 
#define I2C_SCL 9

// UART for Arduino
#define TX1_PIN 17 
#define RX1_PIN 18

// Manual Finger Pads
#define F1 10 
#define F2 11 
#define F3 12 
#define F4 13 

// AI Logic Variables
int VOICE_CYCLE_THRESHOLD = 2;
int stableSignCount = 0; 
String lastLabel = "";
int detectionCounter = 0;

Adafruit_MPU6050 mpu;

// PROTOTYPE (Tells the compiler the function exists later)
void runInference();

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);

    delay(2000); 
    Serial.println("--- GLOVE AI: BOOTING ---");

    Wire.begin(I2C_SDA, I2C_SCL);
    if (!mpu.begin(0x68, &Wire)) { 
        Serial.println("MPU check bypassed...");
    }

    pinMode(F1, INPUT_PULLDOWN);
    pinMode(F2, INPUT_PULLDOWN);
    pinMode(F3, INPUT_PULLDOWN);
    pinMode(F4, INPUT_PULLDOWN);

    // Hall Sensors 1-5
    for(int i=1; i<=5; i++) {
        pinMode(i, INPUT);
    }
    
    Serial.println("SYSTEM READY.");
}

void loop() {
    bool s1 = digitalRead(F1);
    bool s2 = digitalRead(F2);
    bool s3 = digitalRead(F3);
    bool s4 = digitalRead(F4);

    String manualLabel = "";
    char manualChar = ' ';

    // Manual Logic
    if (s1 && !s2 && !s3 && !s4) { manualLabel = "Help"; manualChar = 'H'; }
    else if (!s1 && s2 && !s3 && !s4) { manualLabel = "Salam"; manualChar = 'S'; }
    else if (!s1 && !s2 && s3 && !s4) { manualLabel = "Water"; manualChar = 'W'; }
    else if (!s1 && !s2 && !s3 && s4) { manualLabel = "Food"; manualChar = 'F'; }
    else if (s1 && s2 && !s3 && !s4) { manualLabel = "Emergency"; manualChar = 'E'; }
    else if (s1 && !s2 && s3 && !s4) { manualLabel = "Toilet"; manualChar = 'T'; }
    else if (s1 && !s2 && !s3 && s4) { manualLabel = "Medicine"; manualChar = 'M'; }
    else if (!s1 && s2 && s3 && !s4) { manualLabel = "Thank You"; manualChar = 'Y'; }
    else if (!s1 && s2 && !s3 && s4) { manualLabel = "Yes"; manualChar = 'V'; }
    else if (!s1 && !s2 && s3 && s4) { manualLabel = "No"; manualChar = 'N'; }
    else if (s1 && s2 && s3 && !s4) { manualLabel = "Doctor"; manualChar = 'D'; }
    else if (s1 && s2 && !s3 && s4) { manualLabel = "Pain"; manualChar = 'P'; }
    else if (s1 && !s2 && s3 && s4) { manualLabel = "Sleep"; manualChar = 'L'; }
    else if (!s1 && s2 && s3 && s4) { manualLabel = "I Love You"; manualChar = 'I'; }
    else if (s1 && s2 && s3 && s4) { manualLabel = "Danger"; manualChar = 'X'; }

    if (manualLabel != "") {
        Serial.printf("SIGN: %s (99%%)\n", manualLabel.c_str());
        Serial1.print(manualChar); 
        delay(1500); 
    } 
    else {
        runInference();
    }
}

void runInference() {
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

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
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            if (result.classification[ix].value > 0.85) {
                String currentLabel = result.classification[ix].label;
                Serial.printf("SIGN: %s (%d%%)\n", currentLabel.c_str(), (int)(result.classification[ix].value * 100));

                if (currentLabel == lastLabel) detectionCounter++;
                else { detectionCounter = 1; lastLabel = currentLabel; }

                if (detectionCounter >= 2) {
                    stableSignCount++;
                    detectionCounter = 0; 
                    if (stableSignCount >= VOICE_CYCLE_THRESHOLD) {
                        if (currentLabel == "help") Serial1.print('H');
                        else if (currentLabel == "salam") Serial1.print('S');
                        else if (currentLabel == "water") Serial1.print('W');
                        stableSignCount = 0; 
                    }
                }
            }
        }
    }
}