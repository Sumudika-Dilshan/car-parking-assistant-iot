// Sensor ESP32 code for IoT Car Parking Assistance System sensor node
// Upload this code to each sensor node and configure SENSOR_ID and pins accordingly

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Replace with your main ESP32 MAC address (from Serial Monitor on main ESP32)
// Format as hex bytes (6 elements)
uint8_t mainESP32Address[] = {0xEC, 0xE3, 0x34, 0xB2, 0xED, 0x34};  // <-- UPDATE this MAC address!

// Configure this sensor node's ID (1=Front, 2=Rear, 3=Left, 4=Right)
#define SENSOR_ID 1          // <-- Change this per sensor node

// Ultrasonic sensor pins
#define TRIG_PIN 5           // Trigger pin for HC-SR04
#define ECHO_PIN 18          // Echo pin for HC-SR04

// Data packet structure matching main ESP32
typedef struct struct_message {
  int id;
  float distance;
} struct_message;

struct_message sensorData;

float lastDistance = -1;
unsigned long lastSendTime = 0;
const int sendInterval = 1000;  // Interval in ms between sends

int failedSends = 0;

// Callback called after sending ESP-NOW data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Success");
    failedSends = 0;  // reset failures on success
  } else {
    Serial.println("Failed");
    failedSends++;
    if (failedSends > 5) {
      Serial.println("Too many failures, restarting ESP-NOW");
      esp_now_deinit();
      delay(1000);
      initializeESPNOW();
      failedSends = 0;
    }
  }
}

// Initialize ESP-NOW and add main ESP32 as peer
bool initializeESPNOW() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed!");
    return false;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mainESP32Address, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add main ESP32 as peer");
    return false;
  }
  return true;
}

// Read distance from ultrasonic sensor (HC-SR04)
// Returns distance in cm or -1 if invalid reading
float readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Timeout ~25ms for max 400cm
  long duration = pulseIn(ECHO_PIN, HIGH, 25000);

  if (duration == 0) return -1;  // no echo

  float distance = (duration * 0.034) / 2;  // cm (speed of sound ~0.034 cm/us)

  // Check sensor range validity (2cm to 400cm)
  if (distance < 2 || distance > 400) return -1;

  return distance;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.print("Sensor Node ID: ");
  Serial.println(SENSOR_ID);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Serial.print("This Sensor MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Main ESP32 MAC: ");
  for (int i = 0; i < 6; i++) {
    if (mainESP32Address[i] < 16) Serial.print("0");
    Serial.print(mainESP32Address[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Initialize ESP-NOW
  Serial.println("Initializing ESP-NOW...");
  int attempts = 0;
  while (!initializeESPNOW() && attempts < 5) {
    attempts++;
    Serial.print("Attempt ");
    Serial.print(attempts);
    Serial.println(" failed, retrying in 2 seconds...");
    delay(2000);
  }

  if (attempts >= 5) {
    Serial.println("Failed initialization after 5 attempts. Restarting...");
    delay(5000);
    ESP.restart();
  }
  
  Serial.println("ESP-NOW initialized successfully.");
  Serial.println("Sending sensor readings every 1000 ms...");
  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSendTime >= sendInterval) {
    float totalDistance = 0;
    int validReadings = 0;

    // Take multiple readings for accuracy
    for (int i = 0; i < 3; i++) {
      float reading = readUltrasonicDistance();
      if (reading > 0) {
        totalDistance += reading;
        validReadings++;
      }
      delay(50);
    }

    float distance = -1;
    if (validReadings > 0) {
      distance = totalDistance / validReadings;
      lastDistance = distance;
    } else if (lastDistance > 0) {
      distance = lastDistance;
      Serial.println("Using last valid reading");
    } else {
      Serial.println("No valid sensor readings available");
    }

    // Print reading status to Serial Monitor
    if (distance > 0) {
      Serial.print("Distance: ");
      Serial.print(distance, 1);
      Serial.print(" cm ");
      if (distance >= 100) Serial.println("[SAFE]");
      else if (distance >= 50) Serial.println("[CAUTION]");
      else Serial.println("[DANGER]");
    }

    // Prepare and send data
    sensorData.id = SENSOR_ID;
    sensorData.distance = distance;

    Serial.print("Sending data to main ESP32... ");
    esp_err_t result = esp_now_send(mainESP32Address, (uint8_t *)&sensorData, sizeof(sensorData));
    if (result == ESP_OK) Serial.println("Queued");
    else {
      Serial.println("Failed");
      failedSends++;
    }

    lastSendTime = currentTime;
    Serial.println("---");
  }

  // Optional: system health check every 30 sec
  static unsigned long lastHealthCheck = 0;
  if (millis() - lastHealthCheck > 30000) {
    Serial.print("Failed sends: ");
    Serial.println(failedSends);
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("WiFi channel: ");
    Serial.println(WiFi.channel());
    lastHealthCheck = millis();
  }

  delay(100);  // Stability delay
}
