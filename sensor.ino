#include <WiFiNINA.h>      // Include the WiFiNINA library for Wi-Fi connectivity
#include <PubSubClient.h>  // Include the PubSubClient library for MQTT functionality

// WiFi credentials
const char* ssid = "vivo Y100A";      // Wi-Fi SSID (network name)
const char* password = "12345678";    // Wi-Fi password

// MQTT broker details
const char* mqttServer = "broker.emqx.io";  // Address of the MQTT broker
const int mqttPort = 1883;                  // Port number for the MQTT broker
const char* mqttTopicWave = "SIT210/wave92"; // MQTT topic for wave messages
const char* mqttTopicPat = "SIT210/pat92";   // MQTT topic for pat messages

WiFiClient espClient;          // WiFi client instance for network communication
PubSubClient client(espClient); // MQTT client instance using the WiFi client

// Pin definitions
const int trigPin = 10; // GPIO pin number connected to the ultrasonic sensor's trigger
const int echoPin = 9;  // GPIO pin number connected to the ultrasonic sensor's echo

void setup() {
  // Initialize the serial monitor for debugging and logging
  Serial.begin(115200);

  // Set up the trigger and echo pins for the ultrasonic sensor
  pinMode(trigPin, OUTPUT); // Set trigger pin as output to send ultrasonic pulses
  pinMode(echoPin, INPUT);  // Set echo pin as input to receive reflected pulses

  // Connect to the Wi-Fi network
  connectToWiFi();

  // Set up the MQTT server details
  client.setServer(mqttServer, mqttPort);

  // Connect to the MQTT broker
  connectToMQTT();
}

void loop() {
  // Check if the MQTT client is still connected; if not, attempt to reconnect
  if (!client.connected()) {
    connectToMQTT();
  }
  
  // Maintain the MQTT connection and process incoming messages
  client.loop();

  // Simulate pat detection for demonstration purposes
  if (detectPat()) {
    Serial.println("Pat detected. Publishing 'pat'...");
    // Publish a message to the MQTT broker indicating a "pat" action was detected
    client.publish(mqttTopicPat, "Manmeet Pat");
    delay(5000); // Wait 5 seconds before the next loop iteration
  } else {
    Serial.println("Wave detected. Publishing 'wave'...");
    // Publish a message to the MQTT broker indicating a "wave" action was detected
    client.publish(mqttTopicWave, "Manmeet wave");
    delay(5000); // Wait 5 seconds before the next loop iteration
  }
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  delay(10); // Short delay before starting Wi-Fi connection process
  Serial.println("Connecting to WiFi...");
  
  // Begin Wi-Fi connection using provided SSID and password
  WiFi.begin(ssid, password);
  
  // Wait until the device successfully connects to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Wait 1 second between connection attempts
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

// Function to connect to the MQTT broker
void connectToMQTT() {
  // Attempt to connect to the MQTT broker until successful
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    
    // Attempt to connect with a unique client identifier
    if (client.connect("SenderClient")) {
      Serial.println("Connected to MQTT");
    } else {
      // Delay before retrying the connection
      delay(5000);
    }
  }
}

// Function to detect a "pat" action using the ultrasonic sensor
bool detectPat() {
  // Send a low pulse to clear the sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Send a high pulse for 10 microseconds to trigger the ultrasonic pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the time duration of the echo pulse
  long duration = pulseIn(echoPin, HIGH);
  // Calculate the distance based on the duration of the echo pulse
  int distance = duration * 0.034 / 2;

  // Determine whether the detected object is a "pat" (distance > 20) or a "wave"
  if (distance > 20) {
    return true; // Indicates "pat" action detected
  }
  
  // Return false if the detected object is closer than 20 cm, indicating a "wave"
  if (distance > 0 && distance < 20) {
    return false; // Indicates "wave" action detected
  }
}
