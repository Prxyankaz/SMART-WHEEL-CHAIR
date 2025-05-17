
# Smart Wheelchair IoT System 

A comprehensive assistive technology solution designed to enhance the **safety, independence, and health monitoring** of elderly and specially-abled individuals using IoT, edge computing, and machine learning.

---

## Overview

This Smart Wheelchair system integrates real-time **fall detection**, **vital sign monitoring**, and **location tracking** using multiple sensors, processed locally via the **ESP32-WROOM-32** microcontroller and visualized through a **Flutter-based mobile/web app**. It features both **immediate edge-side alerts** and **cloud-side intelligence** for long-term predictive insights.

---

## System Architecture (IoT Level 5)

* **Perception Layer**:

  * **MPU6050** – Fall detection
  * **MAX30100** – Heart rate and SpO2 monitoring
  * **NEO-6M GPS** – Real-time location tracking
  * **Ultrasonic Sensor** - Object Detection

* **Network Layer**:

  * ESP32 communication via **MQTT/HTTP over Wi-Fi**

* **Data Processing Layer (Edge)**:

  * Preprocessing, noise filtering, and local alert logic

* **Cloud Layer**:

  * **Firebase** for real-time data storage, user auth, alerts, and ML-based analytics

* **Application Layer**:

  * Cross-platform Flutter dashboard for data visualization
  * Mobile app developed in Android Studio for real-time alerts and location tracking

---

## Hardware & Software Stack

### Sensors & Actuators

* **MPU6050** – Motion/orientation sensor
* **MAX30100** – Biomedical sensor for vitals
* **NEO-6M GPS** – Geo-tracking
* **Ultrasonic Sensor** - Object Detection
* **Buzzer** – Audible alerts
* **ESP32-WROOM-32** – Edge device and comms hub

### Software Tools

* **Arduino IDE** – Firmware development
* **Flutter** – Web application dashboard
* **Android Studio** - Mobile app development
* **Firebase** – Realtime DB, Auth, Functions, Hosting
* **ML Models** – Random Forest, XGBoost for predictive insights

---

## Intelligence at the Edge & Cloud

### At the Edge (ESP32):

* Multi-stage fall verification algorithm
* Local vitals threshold checks
* Offline alerting & data buffering
* Buzzer-triggered emergency response

### At the Cloud:

* **Firebase ML + Functions** for:

  * Predictive health insights
  * Alert automation
  * Secure caregiver access
  * Visualization of ROC, trends, and model accuracy

---

## Dashboards & UI

* Real-time heart rate and SpO2 monitoring
* GPS-based wheelchair tracking
* Historical trends and anomaly highlighting
* Mobile app notifications for critical events
* Role-based web access for caregivers
* Accessible UI (high contrast, speech-friendly)

---

## Outcomes

* 95%+ accuracy in fall detection
* Continuous vitals tracking with secure cloud sync
* Real-time alerting within seconds of critical events
* ML-driven health risk predictions (heart-risk)
* Consistent performance in various testing environments

---

## Getting Started

1. **Clone the Repo**

   ```bash
   git clone https://github.com/Prxyankaz/SMART-WHEEL-CHAIR.git
   ```

2. **Hardware Setup**

   * Connect the sensors and buzzer with ESP32
   * Mount the setup on a wheelchair

3. **Flutter App**

   * Open in VScode
   * Install all dependencies
   * Configure Firebase credentials
   * Run and deploy

4. **Mobile App**
   
   * Open in Android Studio
   * Connect Firebase (Ensure google-services.json is present in the app/ directory and matches your Firebase project)
   * Sync Gradle & Run
     

6. **Web Interface**
   
   * Deploy to Firebase Hosting or local server
  
8. **Mobile app Interface**
   
   * Connect a Device or Emulator - Launch a virtual device via AVD Manager, or connect your Android phone with USB Debugging enabled.

---

## Contributors

* Harini Priyanka W (CB.EN.U4CSE22018)
* Mogitha S M (CB.EN.U4CSE22027)
* Prahalyaa A (CB.EN.U4CSE22432)

---

## License

This project is licensed under the MIT License – feel free to build upon it for academic or nonprofit purposes.

---
