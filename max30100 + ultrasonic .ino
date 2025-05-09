>>>>>heart+distance
#include <Wire.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "MAX30100_PulseOximeter.h"

// WiFi Credentials
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Firebase Credentials
#define FIREBASE_HOST ""  // Removed trailing slash
#define FIREBASE_AUTH "" 

// Pulse Oximeter Settings
#define REPORTING_PERIOD_MS 1000

// Ultrasonic Sensor Pins
const int trigPin = 5;
const int echoPin = 18;
const int ledPin = 23;

// Distance thresholds (in cm)
const int CLOSE_DISTANCE = 15;  // Fast blink threshold
const int NEAR_DISTANCE = 30;   // Slow blink threshold

// Firebase and PulseOximeter instances
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

PulseOximeter pox;
uint32_t tsLastReport = 0;
bool firebaseInitialized = false;

// Callback function when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // Return distance in cm
}

void setup() {
    Serial.begin(115200);
    
    // Initialize Ultrasonic Sensor pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // Ensure LED starts off
    
    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    config.timeout.serverResponse = 10 * 1000; // 10 seconds timeout

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    
    // Wait for Firebase connection
    Serial.print("Connecting to Firebase...");
    unsigned long startTime = millis();
    while (!Firebase.ready() && millis() - startTime < 10000) {
        Serial.print(".");
        delay(500);
    }
    
    if (Firebase.ready()) {
        firebaseInitialized = true;
        Serial.println("SUCCESS");
    } else {
        Serial.println("FAILED");
    }

    // Initialize the PulseOximeter instance
    Serial.print("Initializing pulse oximeter...");
    if (!pox.begin()) {
        Serial.println("FAILED");
        for (;;);
    } else {
        Serial.println("SUCCESS");
    }

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void sendToFirebase(float heartRate, float spo2) {
    if (!firebaseInitialized) {
        Serial.println("Firebase not initialized, skipping send");
        return;
    }

    String timeStamp = String(millis());
    FirebaseJson json;
    
    if (heartRate > 0) {
        json.set("heartRate", heartRate);
    }
    
    if (spo2 > 0) {
        json.set("SpO2", spo2);
    }

    if (Firebase.setJSON(firebaseData, "/readings/" + timeStamp, json)) {
        Serial.println("Data sent to Firebase successfully");
    } else {
        Serial.print("Failed to send data: ");
        Serial.println(firebaseData.errorReason());
        
        // Print additional error information
        Serial.print("Firebase error code: ");
        Serial.println(firebaseData.errorCode());
        Serial.print("HTTP code: ");
        Serial.println(firebaseData.httpCode());
    }
}

void loop() {
    // Update PulseOximeter readings
    pox.update();

    // Handle ultrasonic sensor and LED
    float distance = getDistance();
    
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // LED control logic
    if (distance <= CLOSE_DISTANCE && distance > 0) {
        // Fast blink (200ms interval)
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
        Serial.println("TOO CLOSE! Fast blinking");
    } 
    else if (distance <= NEAR_DISTANCE && distance > 0) {
        // Slow blink (500ms interval)
        digitalWrite(ledPin, HIGH);
        delay(250);
        digitalWrite(ledPin, LOW);
        delay(250);
        Serial.println("Approaching! Slow blinking");
    }
    else {
        // No blinking when beyond thresholds
        digitalWrite(ledPin, LOW);
        delay(100); // Small delay between readings
    }

    // Handle Pulse Oximeter data reporting
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float heartRate = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        Serial.print("Heart rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm / SpO2: ");
        Serial.print(spo2);
        Serial.println(" %");

        // Only send to Firebase if we have valid readings
        if (heartRate > 0 || spo2 > 0) {
            sendToFirebase(heartRate, spo2);
        } else {
            Serial.println("Invalid readings, skipping Firebase send");
        }

        tsLastReport = millis();
    }
}
