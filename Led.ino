#include <WiFiNINA.h>      // Include the WiFiNINA library for Wi-Fi connectivity
#include <PubSubClient.h>  // Include the PubSubClient library for MQTT functionality

// WiFi credentials
const char* ssid = "vivo Y100A";      // Wi-Fi SSID (network name)
const char* password = "12345678";    // Wi-Fi password

// MQTT broker details
const char* mqttServer = "broker.emqx.io"; // Address of the MQTT broker
const int mqttPort = 1883;                 // Port number for the MQTT broker
const char* mqttTopicWave = "SIT210/wave92"; // MQTT topic for wave messages
const char* mqttTopicPat = "SIT210/pat92";   // MQTT topic for pat messages

WiFiClient espClient;          // WiFi client instance for network communication
PubSubClient client(espClient); // MQTT client instance using the WiFi client

// Pin definition for LED
const int ledPin = 5; // GPIO pin number connected to the LED

void setup() {
  // Initialize the serial monitor for debugging and logging
  Serial.begin(115200);

  // Set the LED pin mode to output for controlling the LED state
  pinMode(ledPin, OUTPUT);

  // Connect to the Wi-Fi network
  connectToWiFi();

  // Set up the MQTT server details and callback function for message handling
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

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
    if (client.connect("ReceiverClient")) {
      Serial.println("Connected to MQTT");

      // Subscribe to specified topics to receive messages
      client.subscribe(mqttTopicWave);
      client.subscribe(mqttTopicPat);
    } else {
      // Delay before retrying the connection
      delay(5000);
    }
  }
}

// Callback function executed when a message is received on subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  
  // Print the received message payload
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check if the message topic matches "wave" and flash LED 3 times
  if (strcmp(topic, mqttTopicWave) == 0) {
    Serial.println("Wave message received. Flashing LED 3 times...");
    flashLED(3);
  } 
  // Check if the message topic matches "pat" and flash LED 5 times
  else if (strcmp(topic, mqttTopicPat) == 0) {
    Serial.println("Pat message received. Flashing LED 5 times...");
    flashLED(5);  // Different flash pattern for "pat"
  }
}

// Function to flash the LED a specified number of times
void flashLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH); // Turn LED on
    delay(500);                 // Wait for 500 milliseconds
    digitalWrite(ledPin, LOW);  // Turn LED off
    delay(500);                 // Wait for another 500 milliseconds
  }
}
