#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <DeafProject_inferencing.h> 

#define I2C_SDA 8 
#define I2C_SCL 9

Adafruit_MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    while (!Serial); 
    delay(2000); 
    Serial.println("--- GLOVE AI: BOOTING ---");

    Wire.begin(I2C_SDA, I2C_SCL);

    if (!mpu.begin(0x68, &Wire)) { 
        Serial.println("Warning: Library reported failure, but continuing...");
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    for(int i = 1; i <= 5; i++) {
        pinMode(i, INPUT);
    }
    Serial.println("SYSTEM READY. START SIGNING.");
}

void loop() {
    float buffer[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 11) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        // --- THE "BEAST MODE" DATA MAPPING ---
        // Match the order of your Edge Impulse "Axes" list exactly.
        // Assuming: accX, accY, accZ, gyrX, gyrY, gyrZ, hall1-5
        
        buffer[ix + 0] = a.acceleration.x; 
        buffer[ix + 1] = a.acceleration.y;
        buffer[ix + 2] = a.acceleration.z;
        buffer[ix + 3] = g.gyro.x;
        buffer[ix + 4] = g.gyro.y;
        buffer[ix + 5] = g.gyro.z;
        
        // Use map or simple division to ensure Hall sensors hit 0.0 to 1.0
        // If your sensors range from 1800 to 3500, we normalize them here:
        buffer[ix + 6] = (float)analogRead(1) / 4095.0;
        buffer[ix + 7] = (float)analogRead(2) / 4095.0;
        buffer[ix + 8] = (float)analogRead(3) / 4095.0;
        buffer[ix + 9] = (float)analogRead(4) / 4095.0;
        buffer[ix + 10] = (float)analogRead(5) / 4095.0;

        while (micros() < next_tick);
    }

    signal_t signal;
    numpy::signal_from_buffer(buffer, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

    ei_impulse_result_t result = { 0 };
    if (run_classifier(&signal, &result, false) == EI_IMPULSE_OK) {
        bool found = false;
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            // Check confidence > 70% and ignore 'idle'
            if (result.classification[ix].value > 0.70) {
                if (String(result.classification[ix].label) != "idle") {
                    Serial.printf("SIGN: %s (%d%%)\n", 
                                  result.classification[ix].label, 
                                  (int)(result.classification[ix].value * 100));
                    found = true;
                }
            }
        }
        // If the AI is only seeing 'idle', it won't print anything because of the filter above.
    }
}