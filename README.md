# esp32-room-monitor

## An ESP32 project that uses a few sensors to monitor light, temperature, humidity, and motion in a room.

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white) ![Arduino IDE](https://img.shields.io/badge/Arduino%20IDE-%2300979D.svg?style=for-the-badge&logo=Arduino&logoColor=white) ![KiCad](https://img.shields.io/badge/kicad-%23314CB0.svg?style=for-the-badge&logo=kicad&logoColor=white)

**Code may be updated to PlatformIO later on**

<div align="center">
    <img width="700" src="images/completed build/Completed Build 1 (without enclosure).jpg">
    <img width="700" src="images/home assistant/dashboard.png" />
</div>

## ✨ About the project

This project was made to capture data from inside a room and send that data to Home Assistant. (light status, temperature, humidity, and presense) By using a few automations, I was able to devices turn on in a room when someone entered, turn on the fan when it was too hot, etc. It uses an ESP32 DevKit V1 DOIT, a few sensors, and some decoupling capacitors. It works by getting sensor data, proceesing it, and sending it to Home Assistant via MQTT. I could have used ESPHome to create the code, but I decided to do it myself. Here's what the code can do:

* Capture light level via an LDR in a voltage divider with a low pass filter capacitor
* Capture temperature and humidity with a DHT11 (and maybe DHT22)
* Capture presense/motion via the LD2410C
* Have code wirelessly with Arduino OTA
* Send over data with MQTT

## ⚡ Schematic

<div align="center">
    <img width="700" src="images/schematic/Schematic 12_13_25.png" />
</div>
