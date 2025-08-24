#ifndef SENSITIVE_INFO_H
#define SENSITIVE_INFO_H

#include <Arduino.h>

//wifi info
#define WIFI_SSID "DG860A72"
#define WIFI_PASS "7147225282"

//mqtt info
#define MQTT_SERVER_IP "192.168.7.188"
#define MQTT_USER "MQTT_User"
#define MQTT_PASS "N3xusplay"
#define MQTT_PORT 1883
const String MQTT_CLIENT_ID = "esp32_" + String((uint32_t)ESP.getEfuseMac());

//OTA info
#define OTA_HOSTNAME "ESP_Room_Monitor"
#define OTA_PASS "Ricea1a70179a"

#endif