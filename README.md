# 🧤 Neural Voice: Edge-AI Gesture Synthesis System
**Senior College Division | National Science Fair 2026**

[![Hardware: ESP32-S3](https://img.shields.io/badge/Hardware-ESP32--S3-orange?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![AI: Neural Network](https://img.shields.io/badge/AI-Neural%20Network-blue?style=for-the-badge)](https://www.edgeimpulse.com/)
[![OS: EndeavourOS](https://img.shields.io/badge/Dev_OS-EndeavourOS-purple?style=for-the-badge&logo=endeavouros)](https://endeavouros.com/)

## 📝 Project Abstract
**Neural Voice** is a sophisticated assistive technology designed to convert Sign Language into synthesized speech using **Embedded Machine Learning (TinyML)**. Built on the ESP32-S3 architecture, the system performs high-speed neural network inference locally on the device. This eliminates the need for external cloud processing, ensuring near-instantaneous translation and robust data privacy for the user.

---

## 🛠 Hardware Engineering & Interface Mapping

The project utilizes a **Dual-Node Architecture**. The ESP32-S3 serves as the "Inference Engine," while an Arduino serves as the "Peripheral Audio Controller."

### **1. ESP32-S3 Sensor Fusion (Primary Node)**
The glove integrates 11 degrees of freedom to capture the full complexity of human hand movement.

| Component | Pin | Protocol | Functional Logic |
| :--- | :--- | :--- | :--- |
| **IMU (MPU6050) SDA** | **GPIO 8** | I2C | Tracks X/Y/Z Acceleration & Rotation |
| **IMU (MPU6050) SCL** | **GPIO 9** | I2C | Synchronized Clock for 6-Axis Data |
| **Flex Sensor (Thumb)** | **GPIO 1** | ADC1_CH0 | Analog voltage drop via finger bend |
| **Flex Sensor (Index)** | **GPIO 2** | ADC1_CH1 | Capture finger extension/curvature |
| **Flex Sensor (Middle)** | **GPIO 3** | ADC1_CH2 | Capture finger extension/curvature |
| **Flex Sensor (Ring)** | **GPIO 4** | ADC1_CH3 | Capture finger extension/curvature |
| **Flex Sensor (Pinky)** | **GPIO 5** | ADC1_CH4 | Capture finger extension/curvature |
| **Logic TX (UART)** | **GPIO 43** | Serial | Asynchronous Voice Trigger Byte |

---

## 🧠 System Methodology: How It Works

### **Phase I: Signal Acquisition & Normalization**
Raw data from the MPU6050 and the five flex sensors are sampled at a specific frequency (Hz). This data is normalized into a temporal window, creating a "snapshot" of the gesture that the AI can interpret.

### **Phase II: The Neural Network (Inference)**
The core of **Neural Voice** is a pre-trained Neural Network Classifier. 
* **Input Layer:** 11-channel sensor data.
* **Processing:** The model analyzes patterns in motion and finger geometry to find the statistical probability of a match.
* **Thresholding:** We utilize a >85% confidence threshold to prevent false positives.

### **Phase III: Temporal Stability & Filter Logic**
To ensure professional-grade accuracy, we implemented a **2-Frame Consensus Filter**. The system ignores transient "noise" and only accepts a gesture if the AI detects it consistently across consecutive frames.

### **Phase IV: Asynchronous Communication Cycle**
A custom `VOICE_CYCLE_THRESHOLD` governs the interaction between the AI and the audio unit:
1. The AI continuously recognizes signs and prints them to the **Visual Interface** (Serial/Display).
2. The **Audio Voice** is triggered only once every 3rd "Stable Arrival."
3. This prevents audio clipping and allows the speaker to finish a 4-second phrase without being interrupted by the next gesture detection.

---

## 💻 Development & Software Stack
* **Operating System:** EndeavourOS (Linux) for high-efficiency build times.
* **Environment:** VS Code + PlatformIO.
* **AI Toolchain:** Edge Impulse DSP & Neural Network Compiler.
* **Language:** Optimized C++ (utilizing ESP32-S3 vector instructions).

---

## 📂 Installation
1.  Clone the repository:
    ```bash
    git clone [https://github.com/Shajidur-Rahman/ScienceFair2026DeafProject.git](https://github.com/Shajidur-Rahman/ScienceFair2026DeafProject.git)
    ```
2.  Connect the ESP32-S3 and Arduino via UART (Pins 43/44).
3.  Ensure a **Common Ground** connection to prevent serial noise.
4.  Flash the firmware using PlatformIO or Arduino IDE.

---

## 👨‍💻 About the Developer
**Shajidur Rahman**
*Senior College Student & Robotics Researcher*
Specializing in Embedded Systems and Edge Computing.

---
> **Neural Voice:** Giving a voice to the silent through the power of Artificial Intelligence.
