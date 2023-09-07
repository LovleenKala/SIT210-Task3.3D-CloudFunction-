#include <ArduinoMqttClient.h>
#include <WiFi.h>
// code by Lovleen Kala
// Define Wi-Fi credentials
const char ssid[] = "Kartik";     // your network SSID (name)
const char pass[] = "87654321"; // your network password

// Define ultrasonic sensor pins
const int trigPin = 2;
const int echoPin = 3;

// Create a WiFi client and an MQTT client
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Define MQTT broker and topic
const char broker[] = "broker.mqttdashboard.com";
const int port = 1883;
const char topic[] = "SIT210/waves";

// Define the interval for distance measurements
const long interval = 1000;
unsigned long previousMillis = 0;

// Function to set up the Wi-Fi connection
void setupWiFi() {
  // Attempt to connect to WiFi network:
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    // Failed to connect, retrying
    Serial.print(".");
    delay(5000);
  }

  Serial.println("Connected to the network");
  Serial.println();
}

// Function to set up the MQTT connection
void setupMQTT() {
  // Attempt to connect to the MQTT broker:
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("Connected to the MQTT broker!");
  Serial.println();
}

// Function to send an MQTT message with distance information
void sendMQTTMessage(float distance) {
  // Send a message to the MQTT broker
  mqttClient.beginMessage(topic);
  mqttClient.print("Lovleen Kala: Wave is detected, ");
  mqttClient.print("Distance: ");
  mqttClient.print(distance);
  mqttClient.endMessage();
  delay(1000);
}

void setup() {
  // Initialize serial and wait for the port to open
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (needed for native USB port only)
  }

  // Initialize Wi-Fi and MQTT
  setupWiFi();
  setupMQTT();
}

void loop() {
  // Call poll() regularly to allow the library to send MQTT keep-alives
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Ultrasonic sensor measurements
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    float duration = pulseIn(echoPin, HIGH);
    float distance = (duration * 0.0343) / 2;
    Serial.print("Distance: ");
    Serial.println(distance);

    // Check if distance is less than 12 units, and if so, send an MQTT message
    if (distance < 12) {
      sendMQTTMessage(distance);
    }

    Serial.println();
  }
}