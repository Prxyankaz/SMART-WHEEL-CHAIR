>>>>>>>..fall detection + gps
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <TinyGPS++.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi Credentials
#define WIFI_SSID "today is monday"
#define WIFI_PASSWORD "mogi1494"

// Firebase credentials
#define FIREBASE_HOST "mpu6050-93957-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "61ppso2HRpNOjDjtoidnWeeVw6ffKPYx4LUBZ4BY"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial GPS_Serial(1);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

const float FALL_THRESHOLD = 3.0;
const float ANGLE_THRESHOLD = 60.0;
const float GYRO_THRESHOLD = 200.0;

void setup() {
    Serial.begin(115200);
    GPS_Serial.begin(9600, SERIAL_8N1, 16, 17);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\n✅ WiFi Connected!");

    firebaseConfig.host = FIREBASE_HOST;
    firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Firebase.reconnectWiFi(true);
    
    timeClient.begin();
    timeClient.update();
    
    if (!mpu.begin()) {
        Serial.println("❌ MPU6050 Not Found!");
        while (1);
    }
    Serial.println("✅ MPU6050 Initialized!");
}

void loop() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    float accelMagnitude = sqrt(pow(accel.acceleration.x, 2) + 
                                pow(accel.acceleration.y, 2) + 
                                pow(accel.acceleration.z, 2));

    float tiltX = atan2(accel.acceleration.y, accel.acceleration.z) * 57.2958;
    float tiltY = atan2(accel.acceleration.x, accel.acceleration.z) * 57.2958;

    bool fallDetected = (accelMagnitude < FALL_THRESHOLD) ||
                        (abs(gyro.gyro.x) > GYRO_THRESHOLD || abs(gyro.gyro.y) > GYRO_THRESHOLD || abs(gyro.gyro.z) > GYRO_THRESHOLD) ||
                        (abs(tiltX) > ANGLE_THRESHOLD || abs(tiltY) > ANGLE_THRESHOLD);

    while (GPS_Serial.available()) {
        gps.encode(GPS_Serial.read());
    }

    float latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
    float longitude = gps.location.isValid() ? gps.location.lng() : 0.0;
    
    timeClient.update();
    unsigned long timestamp = timeClient.getEpochTime();
    
    String path = "/fall_data/" + String(timestamp);
    FirebaseJson json;
    json.set("timestamp", timestamp);
    json.set("accel_x", accel.acceleration.x);
    json.set("accel_y", accel.acceleration.y);
    json.set("accel_z", accel.acceleration.z);
    json.set("gyro_x", gyro.gyro.x);
    json.set("gyro_y", gyro.gyro.y);
    json.set("gyro_z", gyro.gyro.z);
    json.set("fall_alert", fallDetected);
    json.set("latitude", latitude);
    json.set("longitude", longitude);
    
    if (Firebase.pushJSON(firebaseData, path, json)) {
        Serial.println("✅ Data sent to Firebase");
    } else {
        Serial.println("❌ Firebase Error: " + firebaseData.errorReason());
    }
    
    delay(5000);
}
