# 🧤 Project LexiGlove: AI-Edge Sign Language Translation System
**National Science Fair 2026 | Division: Engineering & Assistive Technology**

[![Hardware: ESP32-S3](https://img.shields.io/badge/Hardware-ESP32--S3-orange?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![AI: Edge Impulse](https://img.shields.io/badge/AI-Edge%20Impulse-blue?style=for-the-badge)](https://www.edgeimpulse.com/)
[![OS: EndeavourOS](https://img.shields.io/badge/Dev_OS-EndeavourOS-purple?style=for-the-badge&logo=endeavouros)](https://endeavouros.com/)

## 🚀 Executive Summary
Project LexiGlove is a wearable **Human-Computer Interaction (HCI)** device designed to empower the speech-impaired community. Unlike traditional systems that rely on smartphone apps or cloud processing, LexiGlove utilizes **Edge AI** to perform real-time gesture recognition directly on the hardware. This ensures zero latency, total privacy, and offline functionality.

---

## 🛠 Hardware Architecture & Pin Mapping

The system uses a **Dual-Processor Architecture** to separate high-speed AI computation from audio signal processing.

### **Primary Controller: ESP32-S3 (WROOM-1)**
The "Inference Engine" handles the Neural Network and Sensor Fusion.

| Component | Pin | Protocol | Technical Role |
| :--- | :--- | :--- | :--- |
| **MPU6050 (SDA)** | **GPIO 8** | I2C | 3-Axis Accel + 3-Axis Gyro (Motion) |
| **MPU6050 (SCL)** | **GPIO 9** | I2C | High-speed data sync (400kHz) |
| **Flex Sensor 1** | **GPIO 1** | ADC1_CH0 | Thumb opposition detection |
| **Flex Sensor 2** | **GPIO 2** | ADC1_CH1 | Index finger curvature tracking |
| **Flex Sensor 3** | **GPIO 3** | ADC1_CH2 | Middle finger extension monitoring |
| **Flex Sensor 4** | **GPIO 4** | ADC1_CH3 | Ring finger position tracking |
| **Flex Sensor 5** | **GPIO 5** | ADC1_CH4 | Pinky finger flexion detection |
| **UART TX** | **GPIO 43** | Serial | Asynchronous command transmission |
| **UART RX** | **GPIO 44** | Serial | System debugging and feedback |

---

## 🧠 How It Works: The "Intelligence" Layer

The system operates through four distinct engineering stages:

### **1. Sensor Fusion & Data Acquisition**
The glove captures 11 independent data streams simultaneously. 
* **The IMU (MPU6050)** detects the "Global Orientation" (Is the hand waving? Is it tilted?). 
* **The Hall Effect/Flex Sensors** detect the "Local Geometry" (Which fingers are closed?).
Data is sampled at a precise frequency to create a **Temporal Window** for the AI to analyze.

### **2. Edge AI Inference (TinyML)**
The raw data is fed into a **Neural Network Classifier** trained via Edge Impulse.
* **Signal Processing:** Raw values are normalized to remove sensor noise.
* **Classification:** The AI calculates the probability of a gesture. If the probability is > 85%, a detection is triggered.

### **3. Temporal Filtering (Stability Logic)**
To prevent "Ghost Triggers" (where the AI flickers between signs), we implemented a **2-Frame Consensus Rule**. A sign is only displayed if the AI identifies it twice in a row, ensuring the user has physically committed to the gesture.

### **4. Asynchronous Audio Management**
We utilize a **Cycle-Based Triggering System** (`VOICE_CYCLE_THRESHOLD`). This allows the visual output (Serial Monitor) to run at full speed (60fps) while the audio voice only plays once every few cycles. This prevents the "stuttering effect" common in lower-end translation gloves.

---

## 📂 Software Stack
* **Kernel:** Arduino C++ / ESP-IDF
* **AI Toolchain:** Edge Impulse Studio (Neural Network)
* **Development OS:** EndeavourOS (Linux)
* **IDE:** VS Code with PlatformIO Extension
* **Libraries:** * `Adafruit_MPU6050`: Modified for 0x70 I2C Address compatibility.
    * `final_deaf_inferencing.h`: Optimized C++ library for S3 vector instructions.

---

## ⚙️ Installation
1. Clone this repo to your local machine.
2. Connect ESP32-S3 and Arduino (Ensure **Common Ground**).
3. Update the `VOICE_CYCLE_THRESHOLD` in the code to adjust speech frequency.
4. Compile and flash using PlatformIO.

---

## 👨‍💻 Developer
**Shajidur Rahman**
*Student, Electronics Enthusiast, & Robotics Researcher*
[GitHub Profile](https://github.com/Shajidur-Rahman)

---
> **Project Goal:** To prove that high-end assistive technology can be built with affordable hardware and intelligent software.
