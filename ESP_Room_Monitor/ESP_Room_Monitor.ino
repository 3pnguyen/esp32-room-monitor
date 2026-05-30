#include <ArduinoOTA.h>
#include "customizable-info/sensitive_info.h"
#include "customizable-info/not_sensitive_info.h"
#include "misc/functions.h"
#include <esp_task_wdt.h>

void setup() {
  blink_built_in(true);

  //debug serial monitor
  Debug_Serial.begin(115200);

  //Wathcdog
  esp_task_wdt_init(8, true);
  esp_task_wdt_add(NULL);

  //WiFi
  setup_WiFi(true);

  //OTA
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();

  //MQTT
  connect_MQTT(true);

  //sensors
  setup_LDR();
  setup_LD();
  setup_DHT();

  blink_built_in(false);
}

void loop() {
  delay(20); //delay to make the loop smoother

  esp_task_wdt_reset(); //keeps the watchdog in check
  
  //for maintaining WiFi, OTA, and MQTT
  ArduinoOTA.handle();
  run_MQTT();
  check_WiFi();

  //run the main sensors and send data
  run_DHT();
  run_LDR();
  run_LD();
}