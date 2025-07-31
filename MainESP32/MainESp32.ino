// ====== BLYNK Configuration ======
#define BLYNK_TEMPLATE_ID "TMPL6Hgr3WGEh"
#define BLYNK_TEMPLATE_NAME "CAR ASSISTANT"
#define BLYNK_AUTH_TOKEN "xqO1OZPEo4VGeI9wLKDqFvmqKFLt0ZwD"

// ======= Library Includes for ESP32, Blynk, Web & JSON =======
#include <esp_now.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials
char ssid[] = "Sumu";             // Your WiFi SSID
char pass[] = "Alexsumu1*";       // Your WiFi Password

// ======= GPIO Assignments =======
#define BUZZER_PIN 23             // Buzzer output pin

// ======= Structure for Incoming ESP-NOW Sensor Data =======
typedef struct struct_message {
  int id;                         // Sensor ID (1 to 4)
  float distance;                 // Distance in cm
} struct_message;

// ======= Data Storage =======
struct_message incomingData;

float distances[4] = {-1, -1, -1, -1};
String colors[4] = {"none", "none", "none", "none"};

// ======= Blynk Virtual Pins =======
#define VPIN_FRONT_LCD   V0
#define VPIN_REAR_LCD    V1
#define VPIN_LEFT_LEVEL  V2
#define VPIN_RIGHT_LEVEL V3

WebServer server(80); // Web server on port 80

// Convert distance to color zones
String distanceToColor(float d) {
  if (d < 0) return "none";
  else if (d >= 100) return "green";
  else if (d >= 50) return "yellow";
  else return "red";
}

// Convert distance to level values for widgets (if used)
int distanceToLevelValue(float d) {
  if (d < 0) return 0;
  else if (d < 50) return 150;
  else if (d < 100) return 75;
  else return 25;
}

void setLcdColor(int vpin, const String& color) {
  if (color == "green") Blynk.setProperty(vpin, "color", "#23C48E");
  else if (color == "yellow") Blynk.setProperty(vpin, "color", "#EFB549");
  else if (color == "red") Blynk.setProperty(vpin, "color", "#D3435C");
  else Blynk.setProperty(vpin, "color", "#FFFFFF");
}

void setLevelColor(int vpin, const String& color) {
  if (color == "green") Blynk.setProperty(vpin, "color", "#23C48E");
  else if (color == "yellow") Blynk.setProperty(vpin, "color", "#EFB549");
  else if (color == "red") Blynk.setProperty(vpin, "color", "#D3435C");
  else Blynk.setProperty(vpin, "color", "#B0B0B0");
}

// Update LCD virtual pins (Front and Rear)
void updateLcd(int idx) {
  int vpin = (idx == 0) ? VPIN_FRONT_LCD : VPIN_REAR_LCD;
  String sensorName = (idx == 0) ? "Front" : "Rear";
  String text = sensorName + ": " + String(distances[idx], 1) + " cm\nStatus: " + colors[idx];
  Blynk.virtualWrite(vpin, text);
  setLcdColor(vpin, colors[idx]);
}

// Update Level virtual pins (Left and Right)
void updateLevelV() {
  Blynk.virtualWrite(VPIN_LEFT_LEVEL, distanceToLevelValue(distances[2]));
  setLevelColor(VPIN_LEFT_LEVEL, colors[2]);
  Blynk.virtualWrite(VPIN_RIGHT_LEVEL, distanceToLevelValue(distances[3]));
  setLevelColor(VPIN_RIGHT_LEVEL, colors[3]);
}

// Turn buzzer on/off based on danger zone presence
void updateBuzzer() {
  bool danger = false;
  for (int i = 0; i < 4; i++) {
    if (colors[i] == "red") {
      danger = true;
      break;
    }
  }
  digitalWrite(BUZZER_PIN, danger ? HIGH : LOW);
}

// ESP-NOW callback for receiving data from sensors
void OnDataRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  if (len != sizeof(incomingData)) return;

  memcpy(&incomingData, data, sizeof(incomingData));
  int idx = incomingData.id - 1;
  if (idx < 0 || idx >= 4) return;

  distances[idx] = incomingData.distance;
  colors[idx] = distanceToColor(distances[idx]);

  if (idx <= 1) updateLcd(idx);
  else updateLevelV();

  updateBuzzer();
  
  // Print received data to Serial Monitor
  Serial.printf("Sensor %d: %.1f cm - Status: %s\n", 
                incomingData.id, distances[idx], colors[idx].c_str());
}

// Simple basic dashboard
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Car Assistant System - Backend</title>
<style>
body{font-family:Arial,sans-serif;background:#000;color:#fff;margin:0;padding:20px;text-align:center;}
h1{color:#74b9ff;margin-bottom:20px;}
.info{background:#333;padding:20px;border-radius:10px;margin:20px 0;}
.status{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:15px;margin:20px 0;}
.sensor{background:#444;padding:15px;border-radius:8px;border-left:4px solid #74b9ff;}
.sensor.green{border-left-color:#2ecc71;}
.sensor.yellow{border-left-color:#f39c12;}
.sensor.red{border-left-color:#e74c3c;}
.distance{font-size:1.5em;font-weight:bold;margin:5px 0;}
</style>
</head>
<body>
<h1>🚗 Car Assistant System Backend</h1>
<div class="info">
<p><strong>System Status:</strong> Running</p>
<p><strong>IP Address:</strong> )rawliteral" + WiFi.localIP().toString() + R"rawliteral(</p>
<p><strong>API Endpoint:</strong> /status</p>
<p><strong>Last Boot:</strong> <span id="uptime">Loading...</span></p>
</div>
<div class="status" id="sensors">
<div class="sensor" id="sensor0">
<h3>🔍 Front Sensor</h3>
<div class="distance" id="dist0">-- cm</div>
<div id="status0">No Data</div>
</div>
<div class="sensor" id="sensor1">
<h3>🔙 Rear Sensor</h3>
<div class="distance" id="dist1">-- cm</div>
<div id="status1">No Data</div>
</div>
<div class="sensor" id="sensor2">
<h3>⬅ Left Sensor</h3>
<div class="distance" id="dist2">-- cm</div>
<div id="status2">No Data</div>
</div>
<div class="sensor" id="sensor3">
<h3>➡ Right Sensor</h3>
<div class="distance" id="dist3">-- cm</div>
<div id="status3">No Data</div>
</div>
</div>
<div class="info">
<p>🔄 Auto-refresh every 2 seconds</p>
<p><strong>For Enhanced Dashboard:</strong> Open your separate HTML file</p>
<p><strong>JSON API:</strong> <a href="/status" style="color:#74b9ff;">/status</a></p>
</div>
<script>
function formatUptime(ms){
const seconds=Math.floor(ms/1000);
const minutes=Math.floor(seconds/60);
const hours=Math.floor(minutes/60);
const days=Math.floor(hours/24);
if(days>0)return days+'d '+hours%24+'h '+minutes%60+'m';
if(hours>0)return hours+'h '+minutes%60+'m '+seconds%60+'s';
if(minutes>0)return minutes+'m '+seconds%60+'s';
return seconds+'s';
}
async function updateData(){
try{
const response=await fetch('/status');
const data=await response.json();
const sensors=['front','rear','left','right'];
sensors.forEach((sensor,i)=>{
const value=data[sensor];
const alert=data.alert[sensor];
const distance=value>=0?value.toFixed(1)+' cm':'-- cm';
document.getElementById('dist'+i).textContent=distance;
document.getElementById('status'+i).textContent=alert==='none'?'No Data':alert.toUpperCase();
document.getElementById('sensor'+i).className='sensor '+alert;
});
document.getElementById('uptime').textContent=formatUptime(data.timestamp);
}catch(e){
console.error('Update failed:',e);
document.getElementById('uptime').textContent='Connection Error';
}}
updateData();
setInterval(updateData,2000);
</script>
</body>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Send JSON data at /status
void handleStatus() {
  StaticJsonDocument<512> doc;
  doc["front"] = distances[0];
  doc["rear"] = distances[1];
  doc["left"] = distances[2];
  doc["right"] = distances[3];

  JsonObject alert = doc.createNestedObject("alert");
  alert["front"] = colors[0];
  alert["rear"] = colors[1];
  alert["left"] = colors[2];
  alert["right"] = colors[3];

  // Add timestamp
  doc["timestamp"] = millis();
  
  // Add system status
  bool hasDanger = false;
  for (int i = 0; i < 4; i++) {
    if (colors[i] == "red") {
      hasDanger = true;
      break;
    }
  }
  doc["systemStatus"] = hasDanger ? "DANGER" : "SAFE";

  String response;
  serializeJson(doc, response);

  // Add CORS headers for cross-origin requests
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  server.send(200, "application/json", response);
}

// Handle OPTIONS requests (CORS preflight)
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "text/plain", "");
}

// Handle 404 errors
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Car Assistant System Starting ===");
  
  // Initialize buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Set WiFi mode and channel
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    while (true) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(500);
      digitalWrite(BUZZER_PIN, LOW);
      delay(500);
    }
  }
  
  // Register ESP-NOW callback
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("✅ ESP-NOW initialized successfully");

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.printf("🔄 Connecting to WiFi: %s", ssid);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" ✅ Connected!");
    Serial.printf("📡 IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("🌐 Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("📶 Signal strength: %d dBm\n", WiFi.RSSI());
  } else {
    Serial.println(" ❌ WiFi connection failed!");
  }

  // Setup HTTP server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);
  server.onNotFound(handleNotFound);

  // Start HTTP server
  server.begin();
  Serial.println("🌐 HTTP server started");
  Serial.println("📱 Access backend at: http://" + WiFi.localIP().toString());
  Serial.println("📊 JSON API at: http://" + WiFi.localIP().toString() + "/status");

  // Initialize Blynk
  Serial.println("🔄 Starting Blynk connection...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("✅ Blynk connected");
  
  // Initial sensor state
  for (int i = 0; i < 4; i++) {
    distances[i] = -1;
    colors[i] = "none";
  }
  
  Serial.println("=== System Ready ===");
  Serial.println("Waiting for sensor data...");
}

void loop() {
  Blynk.run();
  server.handleClient();
  
  // Connection monitoring
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) { // Check every 30 seconds
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("⚠ WiFi disconnected, attempting reconnection...");
      WiFi.reconnect();
    }
    lastWiFiCheck = millis();
  }
  
  // System status print every 10 seconds
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 10000) {
    Serial.println("\n--- System Status ---");
    Serial.printf("WiFi: %s | IP: %s\n", 
                  WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected",
                  WiFi.localIP().toString().c_str());
    Serial.printf("Blynk: %s\n", Blynk.connected() ? "Connected" : "Disconnected");
    
    bool anyDanger = false;
    for (int i = 0; i < 4; i++) {
      String sensorNames[] = {"Front", "Rear", "Left", "Right"};
      Serial.printf("%s: %.1fcm (%s) ", 
                    sensorNames[i].c_str(), 
                    distances[i], 
                    colors[i].c_str());
      if (colors[i] == "red") anyDanger = true;
    }
    Serial.printf("\nBuzzer: %s\n", anyDanger ? "ON" : "OFF");
    Serial.println("--------------------");
    
    lastStatusPrint = millis();
  }
  
  delay(10); // Small delay to prevent watchdog issues
}