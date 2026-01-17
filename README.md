# CPC357Assignment2
An IoT-based smart irrigation monitoring system using ESP32, MQTT, MongoDB Atlas time-series, and real-time visualization.

# 🌱 Smart Irrigation IoT System (ESP32 + MQTT + MongoDB Atlas)

This project implements a **Smart Irrigation Monitoring System** using an ESP32 microcontroller.  
The system collects soil moisture and water tank level data, publishes it via **MQTT**, stores it in **MongoDB Atlas Time-Series**, and visualizes trends using **MongoDB Atlas Charts**.

---

## 🚀 System Overview

The system is designed to:
- Monitor **soil moisture** and **water tank level**
- Automatically control a **water pump**
- Store sensor data in the **cloud**
- Visualize historical data
---

## 🧠 System Architecture
## 🛠 Hardware Components

| Component | Description |
|---------|-------------|
| ESP32 | Microcontroller with WiFi |
| Soil Moisture Sensor | Measures soil moisture level |
| Water Level Sensor | Measures water tank level |
| Relay Module | Controls water pump |
| Water Pump | Irrigation actuator |

---

## 💻 Software & Technologies

- **ESP32 (Arduino IDE)**
- **MQTT (Mosquitto)**
- **Python**
- **MongoDB Atlas (Time-Series)**
- **MongoDB Atlas Charts**
- **MongoDB Atlas Triggers (Email Alert)**
- **Google Cloud VM**

---

## Pump Control Logic
| Condition                      | Action   |
| ------------------------------ | -------- |
| Moisture < 40% AND Water > 10% | Pump ON  |
| Moisture ≥ 80% OR Water ≤ 10%  | Pump OFF |

