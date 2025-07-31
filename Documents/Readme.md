# IoT-Based Car Parking Assistance System

## Project Overview
This project implements an IoT-based car parking assistance system using multiple ESP32 microcontrollers and ultrasonic sensors to detect obstacles around a vehicle. It provides real-time visual and audio feedback via a Blynk mobile app dashboard, helping drivers park safely and avoid collisions.

---

## Features
- Four ultrasonic sensors for 360° obstacle detection (front, rear, left, right).
- Wireless communication using ESP-NOW protocol.
- Real-time distance display with color-coded zones (green: safe, yellow: caution, red: danger).
- Buzzer alert activated when obstacle is too close.
- Mobile dashboard built on Blynk app displaying sensor data via LCD and Level widgets.

---

## Folder Structure
- `MainESP32/` : Contains main controller ESP32 code to receive sensor data and communicate with Blynk.
- `SensorESP32_NodeX/` : Contains individual sensor node code (replace X with sensor ID).
- `Documents/` : Project proposal, report, wiring diagrams, and other documentation.
- `Videos/` : Demo video links or files (optional).

---

## How to Upload Code to ESP32

### Main ESP32 Code
1. Open `MainESP32.ino` in Arduino IDE.
2. Replace placeholders:
   - `char ssid[] = "YourWiFiSSID";`  
   - `char pass[] = "YourWiFiPassword";`  
   - `#define BLYNK_AUTH_TOKEN "YourBlynkAuthToken";`
3. Compile and upload the sketch to your main ESP32 device.

### Sensor Node Code
1. Open each sensor node sketch, e.g., `SensorESP32_Node1.ino`.
2. Replace placeholder MAC address:
uint8_t mainESP32Address[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}; // Main ESP32 MAC


With the actual MAC printed by the main ESP32 Serial Monitor.
3. Set the sensor node ID:
#define SENSOR_ID X // 1=Front, 2=Rear, 3=Left, 4=Right

4. Verify ultrasonic sensor trigger and echo pins correspond to your hardware.
5. Compile and upload each sensor sketch to the respective ESP32 sensor node.

---

## How to Test the Project
1. Power the sensor nodes and main ESP32 (e.g., via power bank for prototype or regulated power supply for real deployment).
2. Open Serial Monitor on main ESP32 for connection and sensor data logs.
3. Open Blynk app on your mobile device.
4. Ensure WiFi and Blynk connections are active.
5. Place obstacles near sensors and observe real-time distance updates, color changes, and buzzer alerts.

---

## Notes
- Always update WiFi SSID, password, and Blynk token before uploading.
- Confirm the **main ESP32 MAC address** in each sensor node code matches the actual device MAC.
- The Blynk free-tier limits widgets; upgrade to Pro for detailed dashboards.
- Use stable and appropriate power sources to avoid connection drops.
- For more detailed explanation and documentation, refer to the `Documents/` folder.

---

## Contact and Resources
- GitHub repository: [Your GitHub repo URL here]  
- Demo video: [YouTube link]  
- For questions or support, contact [email or contact info].

---

Thank you for exploring our IoT-Based Car Parking Assistance System!
