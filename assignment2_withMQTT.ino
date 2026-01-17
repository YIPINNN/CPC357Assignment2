#include <WiFi.h>
#include <PubSubClient.h>

/* ================== WiFi ================== */
const char* ssid = "yipin";
const char* password = "yipintan323";

/* ================== MQTT ================== */
const char* mqttServer = "146.148.76.85";
const int mqttPort = 1883;
const char* MQTT_TOPIC = "iot";

/* ================== Pins ================== */
const int moisturePin = 35;
const int waterLevelPin = 34;
const int relayPin = 32;

/* ================== Calibration ================== */
int MinMoistureValue = 4095;
int MaxMoistureValue = 1600;
int MinMoisture = 0;
int MaxMoisture = 100;

int MinDepthValue = 4095;
int MaxDepthValue = 200;
int MinDepth = 0;
int MaxDepth = 100;

/* ================== Thresholds ================== */
const int moistureOnThreshold = 40;
const int moistureOffThreshold = 80;
const int minWaterLevel = 10;

/* ================== State ================== */
int Moisture = 0;
int depth = 0;
bool pumpState = false;
bool autoMode = true;

/* ================== Objects ================== */
WiFiClient espClient;
PubSubClient client(espClient);

/* ================== WiFi Connect ================== */
void setupWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

/* ================== MQTT Callback ================== */
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("MQTT msg: ");
  Serial.println(msg);

  if (msg.indexOf("\"pump\":\"ON\"") >= 0) {
    digitalWrite(relayPin, HIGH);
    pumpState = true;
  }
  else if (msg.indexOf("\"pump\":\"OFF\"") >= 0) {
    digitalWrite(relayPin, LOW);
    pumpState = false;
  }

  if (msg.indexOf("\"mode\":\"AUTO\"") >= 0) autoMode = true;
  if (msg.indexOf("\"mode\":\"MANUAL\"") >= 0) autoMode = false;
}

/* ================== MQTT Reconnect ================== */
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientID = "ESP32-" + WiFi.macAddress();
    if (client.connect(clientID.c_str())) {
      Serial.println("connected");
      client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

/* ================== Setup ================== */
void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  setupWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("Smart Irrigation MQTT Started");
}

/* ================== Loop ================== */
void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  int rawMoisture = analogRead(moisturePin);
  int rawWaterLevel = analogRead(waterLevelPin);

  Moisture = map(rawMoisture, MinMoistureValue, MaxMoistureValue, MinMoisture, MaxMoisture);
  depth = map(rawWaterLevel, MinDepthValue, MaxDepthValue, MinDepth, MaxDepth);

  Moisture = constrain(Moisture, 0, 100);
  depth = constrain(depth, 0, 100);

  if (autoMode) {
    if (Moisture < moistureOnThreshold && depth > minWaterLevel) {
      digitalWrite(relayPin, HIGH);
      pumpState = true;
    } else if (Moisture >= moistureOffThreshold || depth <= minWaterLevel) {
      digitalWrite(relayPin, LOW);
      pumpState = false;
    }
  }

  /* ===== Publish JSON ===== */
  char payload[256];
  snprintf(payload, sizeof(payload),
           "{"
           "\"type\":\"data\",\n"
           "\"moisture\":%d,\n"
           "\"water\":%d,\n"
           "\"pump\":\"%s\",\n"
           "\"mode\":\"%s\""
           "}",
           Moisture, depth,
           pumpState ? "ON" : "OFF",
           autoMode ? "AUTO" : "MANUAL");

  client.publish(MQTT_TOPIC, payload);

  Serial.println(payload);
  delay(5000);
}