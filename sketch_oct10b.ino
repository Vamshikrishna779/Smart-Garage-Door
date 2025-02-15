#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ctime>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Firebase credentials
#define FIREBASE_HOST "https://esp32-garage-1ccb3-default-rtdb.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyCQexswgV8IIEXRFCGKosGzFWdHb766vvo"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Wi-Fi credentials
const char* ssid = "realme";
const char* password = "abcdefgh";

// Pin Definitions
#define TRIG_PIN 4
#define ECHO_PIN 5
#define SERVO_PIN 18

// Define Firebase paths
const char* firebaseDoorStatusPath = "/door_status";
const char* firebaseManualControlPath = "/manual_control";
const char* firebaseHistoryPath = "/history";
const char* firebaseWifiSSIDPath = "/wifi/ssid";
const char* firebaseWifiPasswordPath = "/wifi/password";
const char* firebaseDoorLockPath = "/door_lock";  // New lock feature path

// NTP client setup for time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// Servo object
Servo myServo;

// Door control variables
long duration;
int distance;
bool doorOpen = false;        // Tracks door state (open or closed)
bool doorLocked = false;  // New lock variable
bool manualOverride = false;  // Indicates if the door was manually opened by the app
unsigned long doorOpenTime = 0;
const unsigned long openTimeout = 30000;  // 5 minutes

// MQTT credentials
const char* mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* mqttClientID = "dtx7kky8bwo3910ji6gr";
const char* mqttUsername = "unused";
const char* mqttPassword = "unused";

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Function declarations
void reconnect();
void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance);
void mqttCallback(char* topic, byte* payload, unsigned int length);
void connectToWiFi(const char* newSSID, const char* newPassword);
void checkWifiCredentials();
void openGarageDoor(bool manual = false);
void closeGarageDoor();
void checkFirebaseControl();
void readUltrasonicSensor();
void recordHistory(const char* status);
void checkDoorLockStatus();

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received message on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(180);  // Initial closed position

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  firebaseConfig.database_url = FIREBASE_HOST;
  firebaseConfig.api_key = FIREBASE_API_KEY;
  firebaseAuth.user.email = "vamshikrishna9608@gmail.com";
  firebaseAuth.user.password = "123abc123";

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  timeClient.begin();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);

  Serial.println("Setup completed");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  timeClient.update();

  checkWifiCredentials();
  checkFirebaseControl();
  readUltrasonicSensor();
  checkDoorLockStatus();

  const char* doorStatus = doorOpen ? "open" : "close";
  Firebase.setString(firebaseData, firebaseDoorStatusPath, doorStatus);
  Firebase.setInt(firebaseData, "/door_distance", distance);

  sendToThingsBoardMQTT(doorStatus, distance);

  delay(500);
}

void connectToWiFi(const char* newSSID, const char* newPassword) {
  WiFi.disconnect();
  Serial.print("Connecting to WiFi: ");
  Serial.println(newSSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(newSSID, newPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientID, mqttUsername, mqttPassword)) {
      Serial.println("connected");
      client.subscribe("/manual_control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance) {
  String payload = "{\"door_status\":\"" + String(doorStatus) + "\",\"door_distance\":" + String(doorDistance) + "}";
  client.publish("v1/devices/me/telemetry", payload.c_str());
}
void checkDoorLockStatus() {
  // Check if door lock status has been updated in Firebase
  if (Firebase.getBool(firebaseData, firebaseDoorLockPath)) {
    doorLocked = firebaseData.boolData();  // Get the door lock status
    if (doorLocked) {
      Serial.println("Door is locked, cannot open.");
    } else {
      Serial.println("Door is unlocked, you can open/close it.");
    }
  } else {
    Serial.println("Failed to get door lock status from Firebase.");
  }
}
void openGarageDoor(bool manual) {
  if (doorLocked) {
    Serial.println("Door is locked. Unable to open.");
    return;
  }

  for (int pos = 180; pos >= 0; pos -= 5) {
    myServo.write(pos);
    delay(20);
  }

  doorOpen = true;
  manualOverride = manual;
  doorOpenTime = millis();
  recordHistory("open");
  Serial.println("Door opened");

  Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
}

void closeGarageDoor() {
   // Prevent closing if door is locked
  if (doorLocked) {
    Serial.println("Cannot close door. Door is locked.");
    return;
  }

  for (int pos = 0; pos <= 180; pos += 5) {
    myServo.write(pos);
    delay(20);
  }

  doorOpen = false;
  manualOverride = false;
  recordHistory("close");
  Serial.println("Door closed");

  Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
}


void checkFirebaseControl() {
  // Check for manual open/close commands from Firebase
  if (Firebase.getString(firebaseData, firebaseManualControlPath)) {
    String command = firebaseData.stringData();
    if (command == "open" && !doorOpen) {
      openGarageDoor(true);  // Manual open
    } else if (command == "close" && doorOpen) {
      closeGarageDoor();  // Manual close
    }
  }
}
void readUltrasonicSensor() {
  static unsigned long lastCheckTime = 0;
  const unsigned long checkInterval = 5000;  // Check every 5 seconds

  if (millis() - lastCheckTime >= checkInterval) {
    lastCheckTime = millis();

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    if (distance < 20 && !doorOpen && !manualOverride) {
      openGarageDoor();
    } else if (distance >= 50 && doorOpen && !manualOverride) {
      closeGarageDoor();
    }
  }
}
// void readUltrasonicSensor() {
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2;

//   // Check distance threshold for automatic open/close
//   if (distance < 30 && !doorOpen && !manualOverride) {
//     openGarageDoor();
//   } else if (distance >= 30 && doorOpen && !manualOverride) {
//     closeGarageDoor();
//   }
// }

void recordHistory(const char* status) {
  time_t now = timeClient.getEpochTime();
  struct tm* timeInfo = localtime(&now);

  char dateBuffer[20];
  strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);

  FirebaseJson json;
  json.set("/status", status);
  json.set("/timestamp", dateBuffer);

  Firebase.pushJSON(firebaseData, firebaseHistoryPath, json);

  Serial.print("History recorded: ");
  Serial.print(status);
  Serial.print(" at ");
  Serial.println(dateBuffer);
}

void checkWifiCredentials() {
  if (Firebase.getString(firebaseData, firebaseWifiSSIDPath)) {
    String newSSID = firebaseData.stringData();  // Get new SSID
    if (newSSID != ssid) {
      // Get new password
      if (Firebase.getString(firebaseData, firebaseWifiPasswordPath)) {
        String newPassword = firebaseData.stringData();  // Get new password
        connectToWiFi(newSSID.c_str(), newPassword.c_str());
      }
    }
  }
}


// #include <WiFi.h>
// #include <FirebaseESP32.h>
// #include <ESP32Servo.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>
// #include <PubSubClient.h>
// #include <ctime>

// #include "addons/TokenHelper.h"
// #include "addons/RTDBHelper.h"

// #define FIREBASE_HOST "https://esp32-garage-1ccb3-default-rtdb.firebaseio.com"
// #define FIREBASE_API_KEY "AIzaSyCQexswgV8IIEXRFCGKosGzFWdHb766vvo"

// FirebaseData firebaseData;
// FirebaseConfig firebaseConfig;
// FirebaseAuth firebaseAuth;

// const char* ssid = "realme";
// const char* password = "abcdefgh";

// #define TRIG_PIN 4
// #define ECHO_PIN 5
// #define SERVO_PIN 18

// const char* firebaseDoorStatusPath = "/door_status";
// const char* firebaseManualControlPath = "/manual_control";
// const char* firebaseHistoryPath = "/history";
// const char* firebaseWifiSSIDPath = "/wifi/ssid";
// const char* firebaseWifiPasswordPath = "/wifi/password";
// const char* firebaseDoorLockPath = "/door_lock";  // New lock feature path

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// Servo myServo;

// long duration;
// int distance;
// bool doorOpen = false;
// bool manualOverride = false;
// bool doorLocked = false;  // New lock variable
// unsigned long doorOpenTime = 0;
// const unsigned long openTimeout = 300000;  // 5 minutes

// // MQTT credentials
// const char* mqttServer = "demo.thingsboard.io";
// const int mqttPort = 1883;
// const char* mqttClientID = "dtx7kky8bwo3910ji6gr";
// const char* mqttUsername = "unused";
// const char* mqttPassword = "unused";
// WiFiClient espClient;
// PubSubClient client(espClient);

// void reconnect();
// void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance);
// void mqttCallback(char* topic, byte* payload, unsigned int length);
// void connectToWiFi(const char* newSSID, const char* newPassword);
// void checkWifiCredentials();
// void openGarageDoor(bool manual = false);
// void closeGarageDoor();
// void checkFirebaseControl();
// void readUltrasonicSensor();
// void recordHistory(const char* status);
// void checkDoorLockStatus();

// void setup() {
//   Serial.begin(115200);

//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);

//   myServo.attach(SERVO_PIN);
//   myServo.write(180);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected to WiFi");
//   Serial.println(WiFi.localIP());

//   firebaseConfig.database_url = FIREBASE_HOST;
//   firebaseConfig.api_key = FIREBASE_API_KEY;
//   firebaseAuth.user.email = "vamshikrishna9608@gmail.com";
//   firebaseAuth.user.password = "123abc123";

//   Firebase.begin(&firebaseConfig, &firebaseAuth);
//   Firebase.reconnectWiFi(true);

//   timeClient.begin();

//   client.setServer(mqttServer, mqttPort);
//   client.setCallback(mqttCallback);

//   Serial.println("Setup completed");
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   timeClient.update();

//   checkWifiCredentials();
//   checkFirebaseControl();
//   readUltrasonicSensor();
//   checkDoorLockStatus();

//   if (doorOpen && (millis() - doorOpenTime) >= openTimeout && !manualOverride) {
//     closeGarageDoor();
//   }

//   const char* doorStatus = doorOpen ? "open" : "close";
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, doorStatus);
//   Firebase.setInt(firebaseData, "/door_distance", distance);

//   sendToThingsBoardMQTT(doorStatus, distance);

//   delay(500);
// }

// void connectToWiFi(const char* newSSID, const char* newPassword) {
//   WiFi.disconnect();
//   Serial.print("Connecting to WiFi: ");
//   Serial.println(newSSID);
//   WiFi.begin(newSSID, newPassword);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }

//   Serial.println("\nConnected to WiFi");
//   Serial.print("IP Address:");
//   Serial.println(WiFi.localIP());
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect(mqttClientID, mqttUsername, mqttPassword)) {
//       Serial.println("connected");
//       client.subscribe("/manual_control");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" trying again in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance) {
//   String payload = "{\"door_status\":\"" + String(doorStatus) + "\",\"door_distance\":" + String(doorDistance) + "}";
//   client.publish("v1/devices/me/telemetry", payload.c_str());
// }

// void openGarageDoor(bool manual) {
//   if (doorLocked) {
//     Serial.println("Door is locked. Unable to open.");
//     return;
//   }

//   for (int pos = 180; pos >= 0; pos -= 5) {
//     myServo.write(pos);
//     delay(20);
//   }

//   doorOpen = true;
//   manualOverride = manual;
//   doorOpenTime = millis();
//   recordHistory("open");
//   Serial.println("Door opened");

//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
// }

// void closeGarageDoor() {
//   for (int pos = 0; pos <= 180; pos += 5) {
//     myServo.write(pos);
//     delay(20);
//   }

//   doorOpen = false;
//   manualOverride = false;
//   recordHistory("close");
//   Serial.println("Door closed");

//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
// }

// void checkFirebaseControl() {
//   if (Firebase.getString(firebaseData, firebaseManualControlPath)) {
//     String command = firebaseData.stringData();
//     if (command == "open" && !doorOpen) {
//       openGarageDoor(true);
//     } else if (command == "close" && doorOpen) {
//       closeGarageDoor();
//     }
//   }
// }

// void readUltrasonicSensor() {
//   static unsigned long lastCheckTime = 0;
//   const unsigned long checkInterval = 5000;  // Check every 5 seconds

//   if (millis() - lastCheckTime >= checkInterval) {
//     lastCheckTime = millis();

//     digitalWrite(TRIG_PIN, LOW);
//     delayMicroseconds(2);
//     digitalWrite(TRIG_PIN, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(TRIG_PIN, LOW);

//     duration = pulseIn(ECHO_PIN, HIGH);
//     distance = duration * 0.034 / 2;

//     if (distance < 20 && !doorOpen && !manualOverride) {
//       openGarageDoor();
//     } else if (distance >= 50 && doorOpen && !manualOverride) {
//       closeGarageDoor();
//     }
//   }
// }

// void recordHistory(const char* status) {
//   time_t now = timeClient.getEpochTime();
//   struct tm* timeInfo = localtime(&now);

//   char dateBuffer[20];
//   strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);

//   FirebaseJson json;
//   json.set("/status", status);
//   json.set("/timestamp", dateBuffer);

//   Firebase.pushJSON(firebaseData, firebaseHistoryPath, json);

//   Serial.print("History recorded: ");
//   Serial.print(status);
//   Serial.print(" at ");
//   Serial.println(dateBuffer);
// }

// void checkDoorLockStatus() {
//   if (Firebase.getBool(firebaseData, firebaseDoorLockPath)) {
//     doorLocked = firebaseData.boolData();
//   }
// }


// void openGarageDoor(bool manual) {
//   for (int pos = 180; pos >= 0; pos -= 5) {  // Rotate from closed (180) to open (0) position in steps of 5 degrees
//     myServo.write(pos);
//     delay(20);  // Adjust the delay to control the speed of the rotation
//   }

//   doorOpen = true;
//   manualOverride = manual;  // Set manual override if the app opens the door
//   recordHistory("open");
//   Serial.println("Door opened");

//   // Update Firebase with new state
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
// }

// void closeGarageDoor() {
//   for (int pos = 0; pos <= 180; pos += 5) {  // Rotate from open (0) to closed (180) position in steps of 5 degrees
//     myServo.write(pos);
//     delay(20);  // Adjust the delay to control the speed of the rotation
//   }

//   doorOpen = false;
//   manualOverride = false;  // Reset manual override
//   recordHistory("close");
//   Serial.println("Door closed");

//   // Update Firebase with new state
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
// }

// void openGarageDoor(bool manual) {
//   myServo.write(0);  // Open position
  
//   doorOpen = true;
//   manualOverride = manual;  // Set manual override if the app opens the door
//   recordHistory("open");
//   Serial.println("Door opened");

//   // Update Firebase with new state
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
// }

// void closeGarageDoor() {
//   myServo.write(180);  // Closed position
 
//   doorOpen = false;
//   manualOverride = false;  // Reset manual override
//   recordHistory("close");
//   Serial.println("Door closed");

//   // Update Firebase with new state
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
// }


// #include <WiFi.h>
// #include <Arduino.h>
// #include <FirebaseESP32.h>
// #include <ESP32Servo.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>
// #include <PubSubClient.h>
// #include <ctime>

// //Provide the token generation process info.
// #include "addons/TokenHelper.h"
// //Provide the RTDB payload printing info and other helper functions.
// #include "addons/RTDBHelper.h"
// // Firebase credentials
// #define FIREBASE_HOST "https://esp32-garage-1ccb3-default-rtdb.firebaseio.com" // No https://
// #define FIREBASE_API_KEY "AIzaSyCQexswgV8IIEXRFCGKosGzFWdHb766vvo"

// FirebaseData firebaseData;
// FirebaseConfig firebaseConfig;
// FirebaseAuth firebaseAuth;
// // Insert your network credentials
// const char* ssid = "realme";
// const char* password = "abcdefgh";
// // Pin definitions
// //const int trigPin = 4;
// //const int echoPin = 5;
// //const int servoPin = 18;  // Pin for Servo motor control
// // Pin Definitions
// #define TRIG_PIN 4
// #define ECHO_PIN 5
// //#define RELAY_PIN 23
// #define SERVO_PIN 18 // Pin for Servo motor control

// // Define Firebase paths
// const char* firebaseDoorStatusPath = "/door_status";
// const char* firebaseManualControlPath = "/manual_control";
// const char* firebaseHistoryPath = "/history";
// const char* firebaseWifiSSIDPath = "/wifi/ssid";
// const char* firebaseWifiPasswordPath = "/wifi/password";



// // NTP client setup for time
// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // Update every 60 seconds
// // Servo object
// Servo myServo;
// // Door control variables
// long duration;
// int distance;
// bool doorOpen = false;
// unsigned long doorOpenTime = 0;  // Time when the door was opened (for auto-close)

// // MQTT credentials
// const char* mqttServer = "demo.thingsboard.io";  // ThingsBoard MQTT server
// const int mqttPort = 1883;
// const char* mqttClientID = "dtx7kky8bwo3910ji6gr";  // Device token
// const char* mqttUsername = "unused";  // ThingsBoard uses 'unused'
// const char* mqttPassword = "unused";  // ThingsBoard uses 'unused'

// // MQTT client
// WiFiClient espClient;
// PubSubClient client(espClient);

// // Function declarations
// void reconnect();
// void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance);
// void mqttCallback(char* topic, byte* payload, unsigned int length);
// void connectToWiFi(const char* newSSID, const char* newPassword);
// void checkWifiCredentials();
// void openGarageDoor();
// void closeGarageDoor();
// void checkFirebaseControl();
// void readUltrasonicSensor();
// void recordHistory(const char* status);

// // This function is called whenever an MQTT message is received
// void mqttCallback(char* topic, byte* payload, unsigned int length) {
//   // Print the topic and the received message (for debugging purposes)
//   Serial.print("Received message on topic: ");
//   Serial.println(topic);

//   // Print the payload as a string (you may need to adjust it depending on your payload format)
//   Serial.print("Message: ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();
// }

// void setup() {
//   // Initialize Serial Monitor
//   Serial.begin(115200);

//   // Initialize pins
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);

//   // Attach the servo to the pin and set initial position
//   myServo.attach(SERVO_PIN);
//   myServo.write(0); // Set initial closed position

//   // Connect to Wi-Fi
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }
//   Serial.println("\nConnected to WiFi");
//   Serial.println(WiFi.localIP());

//   // Initialize Firebase
//   firebaseConfig.database_url = FIREBASE_HOST;
//   firebaseConfig.api_key = FIREBASE_API_KEY;
//   firebaseAuth.user.email = "vamshikrishna9608@gmail.com";  // Replace with Firebase Auth email
//   firebaseAuth.user.password = "123abc123";  // Replace with Firebase Auth password

//   Firebase.begin(&firebaseConfig, &firebaseAuth);
//   Firebase.reconnectWiFi(true);

//   // Start the NTP client
//   timeClient.begin();

//   // Setup MQTT connection
//   client.setServer(mqttServer, mqttPort);
//   client.setCallback(mqttCallback);

//   Serial.println("Setup completed");
// }

// void loop() {
//   // Ensure MQTT is connected
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   // Update the time
//   timeClient.update();

//   // Check for new Wi-Fi credentials from Firebase
//   checkWifiCredentials();

//   // Check manual control from Firebase
//   checkFirebaseControl();

//   // Read ultrasonic sensor
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);
//   duration = pulseIn(ECHO_PIN, HIGH);
//   distance = duration * 0.034 / 2;  // Calculate distance in cm

//   // Automatic door control based on ultrasonic distance
//   if (distance < 10 && !doorOpen) {
//     openGarageDoor();
//   } else if (distance > 10 && doorOpen) {
//     closeGarageDoor();
//   }

//   // Check if door is open and time for auto-close
//   if (doorOpen && (millis() - doorOpenTime >= 10000)) {  // 60 seconds
//     closeGarageDoor();
//   }

//   // Update Firebase with door status and distance
//   const char* doorStatus = doorOpen ? "open" : "close";
//   Firebase.setString(firebaseData, firebaseDoorStatusPath, doorStatus);
//   Firebase.setInt(firebaseData, "/door_distance", distance);

//   // Send telemetry data to ThingsBoard via MQTT
//   sendToThingsBoardMQTT(doorStatus, distance);

//   delay(500); // Delay to control loop frequency
// }

// // Function to connect to Wi-Fi
// void connectToWiFi(const char* newSSID, const char* newPassword) {
//   WiFi.disconnect();  // Disconnect from the current network
//   Serial.print("Connecting to WiFi: ");
//   Serial.println(newSSID);
//   WiFi.begin(newSSID, newPassword);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }

//   Serial.println("\nConnected to WiFi");
//   Serial.print("IP Address: ");
//   Serial.println(WiFi.localIP());
// }

// // Function to check and update Wi-Fi credentials from Firebase
// void checkWifiCredentials() {
//   if (Firebase.getString(firebaseData, firebaseWifiSSIDPath)) {
//     String newSSID = firebaseData.stringData();
//     if (newSSID != ssid) {  // If the SSID has changed
//       ssid = newSSID.c_str();
//       Serial.print("New Wi-Fi SSID: ");
//       Serial.println(ssid);
//       Firebase.getString(firebaseData, firebaseWifiPasswordPath);
//       String newPassword = firebaseData.stringData();
//       if (newPassword != password) {  // If the password has changed
//         password = newPassword.c_str();
//         Serial.print("New Wi-Fi Password: ");
//         Serial.println(password);
//         connectToWiFi(ssid, password);  // Reconnect with new credentials
//       }
//     }
//   }
// }
// // Declare a new variable to store the last recorded door status
// String lastRecordedStatus = "none";  // Initially, set it to a non-status value

// void openGarageDoor() {
//   if (lastRecordedStatus != "open") {  // Check if the door is not already recorded as open
//     Serial.println("Opening garage door...");
//     myServo.write(0);  // Set servo to open position (90°)
//     Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
//     doorOpen = true;  // Ensure doorOpen is updated
//     doorOpenTime = millis(); // Set the open time for auto-close functionality
//     // Record history in Firebase
//     recordHistory("open");
//     lastRecordedStatus = "open";  // Update lastRecordedStatus
//   }
//   delay(1000);  // Adjust delay as necessary
// }

// void closeGarageDoor() {
//   if (lastRecordedStatus != "close") {  // Check if the door is not already recorded as closed
//     Serial.println("Closing garage door...");
//     myServo.write(180);  // Set servo to closed position (0°)
//     Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
//     doorOpen = false;  // Ensure doorOpen is updated
//     // Record history in Firebase
//     recordHistory("close");
//     lastRecordedStatus = "close";  // Update lastRecordedStatus
//   }
//   delay(1000);  // Adjust delay as necessary
// }

// // void openGarageDoor() {
// //   Serial.println("Opening garage door...");
// //   myServo.write(90);  // Set servo to open position (90°)
// //   Firebase.setString(firebaseData, firebaseDoorStatusPath, "open");
// //   doorOpen = true;  // Ensure doorOpen is updated
// //   doorOpenTime = millis(); // Set the open time for auto-close functionality
// //   // Record history in Firebase
// //   recordHistory("open");
// //   delay(1000);  // Adjust delay as necessary
// // }

// // void closeGarageDoor() {
// //   Serial.println("Closing garage door...");
// //   myServo.write(0);  // Set servo to closed position (0°)
// //   Firebase.setString(firebaseData, firebaseDoorStatusPath, "close");
// //   doorOpen = false;  // Ensure doorOpen is updated
// //    // Record history in Firebase
// //   recordHistory("close");
// //   delay(1000);  // Adjust delay as necessary
// // }


// // Function to check for manual control via the Firebase app
// void checkFirebaseControl() {
//   if (Firebase.getString(firebaseData, firebaseManualControlPath)) {
//     String manualCommand = firebaseData.stringData();

//     if (manualCommand == "open") {
//       openGarageDoor();
//       // Set Firebase manual control to empty after executing command
//       Firebase.setString(firebaseData, firebaseDoorStatusPath, ""); // Clear command
//     } else if (manualCommand == "close") {
//       closeGarageDoor();
//       // Set Firebase manual control to empty after executing command
//       Firebase.setString(firebaseData, firebaseDoorStatusPath, ""); // Clear command
//     }
//   } else {
//     Serial.println("Failed to read from Firebase");
//     Serial.println("Reason: " + firebaseData.errorReason());  // Print the error reason
//   }
// }
// void recordHistory(const char* status) {
//   time_t now = timeClient.getEpochTime();  // Get current epoch time
//   struct tm* timeInfo = localtime(&now);   // Convert epoch to tm struct

//   char dateBuffer[20];
//   strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);  // Format date and time

//   FirebaseJson json;
//   json.set("/status", status);
//   json.set("/timestamp", dateBuffer);  // Set formatted date and time

//   Firebase.pushJSON(firebaseData, firebaseHistoryPath, json);

//   Serial.print("History recorded: ");
//   Serial.print(status);
//   Serial.print(" at ");
//   Serial.println(dateBuffer);
// }


// // Send data to ThingsBoard via MQTT
// void sendToThingsBoardMQTT(const char* doorStatus, int doorDistance) {
//   String payload = "{\"doorStatus\":\"" + String(doorStatus) + "\",\"doorDistance\":" + String(doorDistance) + "}";
//   client.publish("v1/devices/me/telemetry", payload.c_str());
// }

// // Function to reconnect to MQTT server
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     if (client.connect(mqttClientID, mqttUsername, mqttPassword)) {
//       Serial.println("connected");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       delay(5000);
//     }
//   }
// }