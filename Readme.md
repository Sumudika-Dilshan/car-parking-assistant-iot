# IoT-Based Car Parking Assistance System

## Project Overview
This project presents an IoT-enabled car parking assistance system designed to improve driver safety and convenience. It uses multiple ESP32 microcontrollers paired with ultrasonic sensors around a vehicle to detect obstacles in real-time. 

The system wirelessly communicates sensor data to a main ESP32, which processes and streams the information to a mobile dashboard app. Visual and audio alerts inform the driver of nearby obstacles, significantly reducing parking-related accidents.

---

## Features
- Four ultrasonic sensors positioned at **front**, **rear**, **left**, and **right** of the vehicle for full 360° obstacle detection.
- Wireless sensor data transmission from sensor nodes to main ESP32 via **ESP-NOW** protocol.
- The main ESP32 hosts an **HTTP server** serving live sensor data as **JSON** at `/status` endpoint.
- Real-time mobile dashboard using **Blynk IoT platform** for intuitive visualization and buzzer control.
- Color-coded zones (green/yellow/red) representing proximity safety levels.
- Audible buzzer beeps that increase frequency as obstacles get closer.

---

## System Architecture & Communication

### ESP-NOW Between Sensor Nodes and Main ESP32
The sensor ESP32 nodes use ESP-NOW, a low-latency, connectionless wireless protocol developed by Espressif, to send distance measurements directly to the main ESP32 controller. This ensures efficient local communication without requiring a WiFi router or internet connection in the sensor network.

### HTTP Server on Main ESP32 Serving JSON Data
To satisfy module communication requirements, the main ESP32 runs an HTTP server accessible on the local network at:

http://<ESP32_IP_ADDRESS>/status


This endpoint serves real-time sensor data formatted as JSON:

{
"front": 120.5,
"rear": 98.3,
"left": 34.2,
"right": 51.7,
"alert": {
"front": "green",
"rear": "yellow",
"left": "red",
"right": "green"
}
}


External dashboards or applications can retrieve and parse this data for customized visualization or further processing.

### Blynk Cloud for Mobile Visualization
Simultaneously, the main ESP32 connects to the Blynk cloud service over WiFi. Sensor data is mapped to virtual pins and displayed via Blynk widgets (LCD and Level indicators) on a mobile app, offering real-time visual cues and buzzer control. This provides a user-friendly interface accessible anywhere.

---

## How to Access and Use the HTTP Endpoint

1. Ensure the main ESP32 is connected to your WiFi network.
2. Note the IP address printed via Serial Monitor upon startup.
3. On any device connected to the same WiFi, open a web browser.
4. Navigate to:
http://<ESP32_IP_ADDRESS>/status

5. The browser will display live JSON data with distances and alert statuses.
6. You can use this endpoint as a data source for custom dashboards in Python or other platforms.

---

## Instructions for Setting Credentials and Parameters

Before compiling and uploading:

### In `MainESP32.ino`:
char ssid[] = "YourWiFiSSID"; // <-- Replace with your WiFi network's SSID
char pass[] = "YourWiFiPassword"; // <-- Replace with your WiFi network's password
#define BLYNK_AUTH_TOKEN "YourBlynkAuthToken" // <-- Replace with your Blynk authentication token


### In each Sensor Node's Sketch (`SensorESP32_NodeX.ino`):
uint8_t mainESP32Address[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX}; // <-- Replace with main ESP32's MAC address

#define SENSOR_ID X // 1=Front, 2=Rear, 3=Left, 4=Right; assign unique ID per sensor node


Make sure to accurately configure SSID, password, Blynk token, MAC address, and sensor ID for proper operation.

---

## Folder Structure
A recommended organization for your project repo:

CarParkingAssistant/
├── MainESP32/
│ └── MainESP32.ino
├── SensorESP32_Node1/
│ └── SensorESP32_Node1.ino
├── Documents/
│ ├── ProjectProposal.pdf
│ ├── WiringDiagram.png
│ └── ProjectReport.pdf
├── Videos/
│ └── DemoLink.txt
└── README.md


---

## Getting Started

1. Upload `SensorESP32_NodeX.ino` to each ESP32 sensor node after setting the MAC address and Sensor ID.
2. Upload `MainESP32.ino` to the main ESP32 controller after setting WiFi and Blynk credentials.
3. Power all devices, ensuring sensor nodes and main ESP32 are connected.
4. Use the Blynk app to monitor sensors visually and audibly.
5. Access the HTTP JSON endpoint to integrate with custom dashboards or further software.

---

## Future Work and Extensions

- Develop a Python web dashboard or application to consume the HTTP JSON endpoint.
- Add voice alerts or integrate GPS for enhanced context awareness.
- Expand to additional sensor nodes for more comprehensive coverage.

---

## Contact and Resources

- GitHub Repository: [Your GitHub URL here]
- Demo Video: [YouTube Link here]
- For inquiries, contact: [Your contact information]

---

Thank you for exploring the IoT-Based Car Parking Assistance System!