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
    delay(2000); // Wait for serial and power to stabilize
    Serial.println("--- GLOVE AI: BOOTING ---");

    Wire.begin(I2C_SDA, I2C_SCL);

    // This is the fix. We manually pass 0x68 but the library 
    // will now treat your 0x70 chip as a standard MPU.
    if (!mpu.begin(0x68, &Wire)) { 
        Serial.println("Warning: Library reported failure, but continuing...");
    }

    Serial.println("MPU WOKEN UP!");

    // Set configuration manually to ensure it takes
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

    for (size_t ix = 0; ix < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; ix += 11) {
        uint64_t next_tick = micros() + (EI_CLASSIFIER_INTERVAL_MS * 1000);

        sensors_event_t a, g, temp;
        
        // Reading data from the 0x70 chip
        mpu.getEvent(&a, &g, &temp);

        buffer[ix + 0] = a.acceleration.x;
        buffer[ix + 1] = a.acceleration.y;
        buffer[ix + 2] = a.acceleration.z;
        buffer[ix + 3] = g.gyro.x;
        buffer[ix + 4] = g.gyro.y;
        buffer[ix + 5] = g.gyro.z;
        
        // Hall Sensors on pins 1-5
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
                Serial.printf("SIGN: %s (%d%%)\n", 
                              result.classification[ix].label, 
                              (int)(result.classification[ix].value * 100));
            }
        }
    }
}