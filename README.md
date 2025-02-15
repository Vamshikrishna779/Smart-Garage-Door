# Smart Garage Door Opener

## Overview
The **Smart Garage Door Opener** is an IoT-based automation system designed to enhance security and convenience. This project uses the **ESP32 microcontroller**, **Firebase** for real-time cloud synchronization, **MQTT** for telemetry, **ultrasonic sensors** for automatic control, and a **mobile app** for remote access.

## Features
- **Automatic & Manual Control**: Open/close the garage door using sensors or a mobile app.
- **Real-time Monitoring**: View live door status via Firebase.
- **Secure Access**: Lock mechanism prevents unauthorized access.
- **Mobile App Integration**: Control and monitor operations from anywhere.
- **Dynamic Wi-Fi Updates**: Modify Wi-Fi credentials remotely.
- **NTP Synchronization**: Timestamp tracking for door operations.
- **ThingsBoard Dashboard**: Graphical telemetry visualization.

## Hardware Components
- **ESP32 Microcontroller** (handles all processing and communication)
- **Ultrasonic Sensor** (detects vehicle proximity)
- **Servo Motor** (opens and closes the garage door)
- **Relay Module** (controls the locking mechanism)
- **Power Supply**

## Software & Technologies
- **Arduino IDE** (ESP32 programming)
- **Firebase Realtime Database** (stores door status, logs, and credentials)
- **Android Studio** (mobile app development)
- **ThingsBoard** (visual dashboard for real-time telemetry)

## Installation & Setup
### 1. Clone the Repository
```sh
 git clone https://github.com/your-repo/smart-garage-door.git
```
### 2. ESP32 Firmware Setup
- Install **Arduino IDE**
- Add ESP32 board support
- Install necessary libraries: `WiFi`, `FirebaseESP32`, `PubSubClient`
- Upload the firmware to ESP32

### 3. Firebase Configuration
- Create a Firebase project.
- Enable Realtime Database and Authentication.
- Add `google-services.json` to the mobile app.

### 4. Android App Setup
- Open project in **Android Studio**.
- Configure Firebase credentials.
- Build and deploy the app on your mobile device.

## Usage
1. **Power ON ESP32**: Connect the hardware setup to a power source.
2. **Monitor Door Status**: View live status on the mobile app.
3. **Control Door**:
   - Use the mobile app buttons to open/close the door.
   - Automatic operation via ultrasonic sensor.
4. **View History**: Access past door operations with timestamps.
5. **Modify Wi-Fi Credentials**: Change SSID and password dynamically from the app.

## System Workflow
1. ESP32 connects to **Wi-Fi** and syncs with **Firebase**.
2. Ultrasonic sensor detects a vehicle’s presence.
3. Door opens automatically if proximity conditions are met.
4. Users can **manually open/close** the door from the app.
5. Lock mechanism prevents unauthorized access.
6. Data updates in Firebase, allowing real-time monitoring.
7. ThingsBoard displays telemetry data for visualization.

## Screenshots
<p align="center">
  <img src="https://github.com/user-attachments/assets/example1.png" width="300">
  <img src="https://github.com/user-attachments/assets/example2.png" width="300">
</p>

## Future Enhancements
- Voice assistant integration (Google Assistant/Alexa).
- AI-based face recognition for secure access.
- Battery backup for power failures.

## License
This project is licensed under the **MIT License**. Feel free to modify and contribute.

## Contributors
- **GitHub:** [Vamshikrishna779](https://github.com/Vamshikrishna779)
- **LinkedIn:** [Your LinkedIn Profile](#)
- **Collaborators**

## Contact

