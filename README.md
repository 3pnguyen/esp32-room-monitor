# esp32-room-monitor

## An ESP32 project that uses a few sensors to monitor light, temperature, humidity, and motion in a room.

![Completed Build 1 (without enclosure)](https://github.com/user-attachments/assets/e35e205c-7968-424e-a256-367307a0e299)

<img width="1286" height="851" alt="Home Assistant SS 2" src="https://github.com/user-attachments/assets/24d56b13-5f1e-485e-99d2-aa3e6731779b" />

## ✨ About the project

This project was made to capture data from inside a room and send that data to Home Assistant. (light status, temperature, humidity, and presense) By using a few automations, I was able to devices turn on in a room when someone entered, turn on the fan when it was too hot, etc. It uses an ESP32 DevKit V1 DOIT, a few sensors, and some decoupling capacitors. It works by getting sensor data, proceesing it, and sending it to Home Assistant via MQTT. I could have used ESPHome to create the code, but I decided to do it myself. Here's what the code can do:

* Capture light level via an LDR in a voltage divider with a low pass filter capacitor
* Capture temperature and humidity with a DHT11 (and maybe DHT22)
* Capture presense/motion via the LD2410C
* Have code wirelessly with Arduino OTA
* Send over data with MQTT

## ⚡ Schematic

<img width="1875" height="1337" alt="Schematic PNG" src="https://github.com/user-attachments/assets/743cee9b-19ec-4686-b6c2-3833eabdce51" />

* I don't have a PCB design of this. 😁
