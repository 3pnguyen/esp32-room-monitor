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

## 📋 How to use this

(assuming you already have Home Assistant, the Mosquitto Broker add-on, and the circuit all wired up)

I don't know why you would want to but just in case:

For the ESP32,
1. Download the code and the two header files
2. For the "sensitive_info_example" file, change the name to "sensitive_info"
3. Change "SENSITIVE_INFO_EXAMPLE_H" to "SENSITIVE_INFO_H"
4. In the "sensitive_info" file, put in your information like your network, the IP which your Home Assistant server has, etc.
5. If you change any of the pins, change that in the "not_sensitive_info"
6. Install some of the libraries that the code requires. Mentioned in the code near the top.
7. After, upload the code, and you should be good to go. If the code does work, you will see the on-board LED of the ESP32 turn on and turn off.
8. If you want to adjust some of the sensors, you can do that in the code

<img width="1280" height="866" alt="Home Assistant SS 1" src="https://github.com/user-attachments/assets/c4ba8031-24c4-416f-b3d3-f68643c7dfaa" />

In Home Assistant,
1. Go to your configuration yaml file
2. Copy the image above. If it is not for your bedroom, you can change the name of that

## 🪰 Known issues

* If the senstivity for the LD2410C is too high it can detect other motion, even when someone is not in the room
