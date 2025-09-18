//header file for the non-sensitive but customizable information such as the pin numbers on the ESP32

#ifndef NOT_SENSITIVE_INFO_H
#define NOT_SENSITIVE_INFO_H

//if you want to use different pins or settings for your ESP32, you can easily change them here

//the interval for when the ESP32 will send information via MQTT
#define MQTT_INTERVAL 5000

//the pin number that the on-board LED for the ESP32 uses. May or may not be different for your ESP32
#define LED_BUILTIN 2

//for the DHT11/DHT22 depending on which you will use. My setup uses the DHT11 I had laying around
#define DHT_PIN 4
#define DHT_TYPE DHT11

//for the photoresistor/LDR
#define LDR_PIN 33
#define LIGHT_THRESHOLD 950 //the light threshold before the room is considered to have no light

/*
for the LD2410C
(UART0 -> GPIO 1 (TX) & 3 (RX))
(UART1 -> GPIO 10 (TX) & 9 (RX))
(UART2 -> GPIO 17 (TX) & 16 (RX))
I used UART2
*/
#define LD_RX_PIN 16
#define LD_TX_PIN 17
#define UART_INTERFACE 2
const uint8_t ENERGY_THRESHOLD = 40; //the energy threshold, (0-100) before the room is considered to have no motion


#endif


