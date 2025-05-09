#define FIREBASE_USE_PSRAM
#define FB_ENABLE_ERROR_QUEUE false
#define FB_ENABLE_DEBUG false

#include <Wire.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000
#define ULTRASONIC_UPDATE_MS 500

// WiFi Credentials
#define WIFI_SSID "today is monday"
#define WIFI_PASSWORD "mogi1494"

// Firebase Credentials
#define FIREBASE_HOST "mpu6050-93957-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "61ppso2HRpNOjDjtoidnWeeVw6ffKPYx4LUBZ4BY"

// Ultrasonic Sensor Pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// Firebase and PulseOximeter instances
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

PulseOximeter pox;

uint32_t tsLastReport = 0;
uint32_t tsLastUltrasonic = 0;

// Callback function when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(115200);
    
    // Initialize ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

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

float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2; // Calculate distance in cm
    
    return distance;
}

void loop() {
    // Update PulseOximeter readings
    pox.update();

    // Current timestamp
    unsigned long currentMillis = millis();
    String timeStamp = String(currentMillis);

    // Handle MAX30100 data
    if (currentMillis - tsLastReport > REPORTING_PERIOD_MS) {
        float heartRate = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        Serial.print("Heart rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm / SpO2: ");
        Serial.print(spo2);
        Serial.println(" %");

        // Store data with a timestamped path
        if (heartRate > 0) {
            if (Firebase.setFloat(firebaseData, "/healthData/" + timeStamp + "/heartRate", heartRate)) {
                Serial.println("Heart Rate sent to Firebase");
            } else {
                Serial.print("Failed to send heart rate: ");
                Serial.println(firebaseData.errorReason());
            }
        }

        if (spo2 > 0) {
            if (Firebase.setFloat(firebaseData, "/healthData/" + timeStamp + "/SpO2", spo2)) {
                Serial.println("SpO2 sent to Firebase");
            } else {
                Serial.print("Failed to send SpO2: ");
                Serial.println(firebaseData.errorReason());
            }
        }

        tsLastReport = currentMillis;
    }

    // Handle Ultrasonic sensor data
    if (currentMillis - tsLastUltrasonic > ULTRASONIC_UPDATE_MS) {
        float distance = getDistance();
        
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        if (distance > 0 && distance < 400) { // Valid range for HC-SR04
            if (Firebase.setFloat(firebaseData, "/obstacleData/" + timeStamp + "/distance", distance)) {
                Serial.println("Distance sent to Firebase");
            } else {
                Serial.print("Failed to send distance: ");
                Serial.println(firebaseData.errorReason());
            }
        }
        
        tsLastUltrasonic = currentMillis;
    }
}
