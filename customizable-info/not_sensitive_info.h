#ifndef NOT_SENSITIVE_INFO_H
#define NOT_SENSITIVE_INFO_H

#include "Arduino.h"

//the interval for when the ESP32 will send information via MQTT
#define MQTT_INTERVAL 3000

//the pin number for the on-board LED
#define LED_BUILTIN 2

//for the DHT11/DHT22
#define DHT_PIN 4
#define DHT_TYPE DHT11

//for the photoresistor/LDR
#define LDR_PIN 33
#define LIGHT_THRESHOLD 950

/*
(UART0 -> GPIO 1 (TX) & 3 (RX))
(UART1 -> GPIO 10 (TX) & 9 (RX))
(UART2 -> GPIO 17 (TX) & 16 (RX))
*/
#define LD_RX_PIN 16
#define LD_TX_PIN 17
#define UART_INTERFACE 2
const uint8_t ENERGY_THRESHOLD = 40; //the energy threshold, (0-100) before the room is considered to have no motion

#define Debug_Serial Serial //renames the Serial into more descriptive names
#define LD_Serial Serial2

extern const char* motion_topic;
extern const char* temperature_topic;
extern const char* humidity_topic;
extern const char* light_topic;

#endif