//template for applying sensitive information into code

#ifndef SENSITIVE_INFO_EXAMPLE_H //change this name later to SENSITIVE_INFO_H after renaming the file
#define SENSITIVE_INFO_EXAMPLE_H //this one too

#include <Arduino.h>

//wifi info
#define WIFI_SSID "Your_network_here"
#define WIFI_PASS "Your_network_pass_here"

//mqtt info
#define MQTT_SERVER_IP "MQTT_server_ip_here"
#define MQTT_USER "MQTT_usnerame_here"
#define MQTT_PASS "MQTT_password_here"
#define MQTT_PORT "MQTT_port_here"
const String MQTT_CLIENT_ID = "MQTT_client_ID_here";

//OTA info
#define OTA_HOSTNAME "OTA_hostname_here"
#define OTA_PASS "OTA_password_here"

#endif