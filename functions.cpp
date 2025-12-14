#include <WiFi.h>
#include <PubSubClient.h> //needs to be installed from the library manager
//for DHT
#include <DHT.h> //needs to be installed from the library manager
#include <cmath>
//for LD2410
#include <ld2410.h> //needs to be installed from the library manager
#include <HardwareSerial.h>
//for customizable information
#include "customizable-info/sensitive_info.h"
#include "customizable-info/not_sensitive_info.h"
//custom classes
#include "custom-classes/IntervalTimer.h"
#include "custom-classes/Debounce.h"
#include "custom-classes/EMAFilter.h"

//ESP LED ----------------------------------------------------------------------------------------------------------------------------------------------------------

void blink_built_in(bool onAfterwards, int speed) {
  pinMode(LED_BUILTIN, OUTPUT);

  int wasOn = digitalRead(LED_BUILTIN);

  for (int i = 0; i < 2; i++) {
    digitalWrite(LED_BUILTIN, (wasOn) ? LOW : HIGH);
    delay(speed);
    digitalWrite(LED_BUILTIN, (wasOn) ? HIGH : LOW);
    delay(speed);
  }

  if (onAfterwards) {
    digitalWrite(LED_BUILTIN, OUTPUT);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

//WIFI ----------------------------------------------------------------------------------------------------------------------------------------------------------
#include <WiFi.h>
IntervalTimer WiFiTimer(10000);

void setup_WiFi(bool initialConnection) {
  delay(500);

  WiFiTimer.reset();

  if (initialConnection) {
    WiFi.mode(WIFI_STA);
    Serial.println("\n");
  }

  WiFi.disconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED && !WiFiTimer.isReady()) {
    delay(100);
    if (initialConnection) {
      Debug_Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (initialConnection) {
      Debug_Serial.println("\nWiFi connected");
      Debug_Serial.print("IP address: ");
      Debug_Serial.print(WiFi.localIP());
      Debug_Serial.println("\n");
    }
  } else {
    Debug_Serial.println("Could not connect to WiFi\n");
  }

  delay(1000);
}

void check_WiFi() {
  if(WiFi.status() != WL_CONNECTED) {
    setup_WiFi(false);
  }
}

//MQTT ----------------------------------------------------------------------------------------------------------------------------------------------------------
#include <PubSubClient.h> //needs to be installed from the library manager
WiFiClient espClient; //mandatory setup for connecting to MQTT
PubSubClient client(espClient);

//attempts to connect to the MQTT server
void connect_MQTT(bool initialConnection) {

  if (initialConnection) client.setServer(MQTT_SERVER_IP, MQTT_PORT);

  while(!client.connected()) {
    Debug_Serial.println("Attempting MQTT connection");

    if(client.connect(MQTT_CLIENT_ID.c_str(), MQTT_USER, MQTT_PASS)) {
      Debug_Serial.println("MQTT connected");
      Debug_Serial.print("Client ID: ");
      Debug_Serial.print(MQTT_CLIENT_ID);
      Debug_Serial.println("\n");
    } else {
      Debug_Serial.print("Failed, ");
      Debug_Serial.println("Retry in 5 seconds");

      for(int i = 5; i > 0; i--) {
        Debug_Serial.println(String(i) + "...");
        delay(1000);
      }
   }
  }
}

void run_MQTT() {
  if(!client.connected()) {
    connect_MQTT(false);
  }
  client.loop();
}

//DHT ----------------------------------------------------------------------------------------------------------------------------------------------------------
#include <DHT.h> //needs to be installed from the library manager
DHT dht(DHT_PIN, DHT_TYPE);
IntervalTimer DHTMessageTimer(MQTT_INTERVAL);

void setup_DHT() {
  pinMode(DHT_PIN, INPUT);

  dht.begin();
  delay(2000);

  Debug_Serial.println("DHT11 set up\n");
}

void run_DHT(bool debug) { 
  if (DHTMessageTimer.isReady()) {
    float temperature = dht.convertCtoF(dht.readTemperature());
    float humidity = dht.readHumidity();

    if (!isnan(temperature)) {
      if ((temperature > 50) && (temperature < 100)) {
        String tempStr = String(temperature);
        client.publish(temperature_topic, tempStr.c_str());
      }
    }

    if (!isnan(humidity)) {
      if ((humidity > 10) && (humidity <= 100)) {
        String humidityStr = String(humidity);
        client.publish(humidity_topic, humidityStr.c_str());
      }
    }

    if (debug) {
      Debug_Serial.print("Temperature: ");
      Debug_Serial.print(temperature);
      Debug_Serial.print((!isnan(temperature)) ? "F\n" : "\n");
      Debug_Serial.print("Humidity: ");
      Debug_Serial.print(humidity);
      Debug_Serial.println((!isnan(humidity)) ? "%\n" : "\n");
    }
  }
}

//Photoresistor ----------------------------------------------------------------------------------------------------------------------------------------------------------
IntervalTimer LDRMessageTimer(MQTT_INTERVAL);
Debounce<bool> LDRDebounce(false);
EMAFilter<int> LDRFilter(0.2, 0.1, LIGHT_THRESHOLD);

void run_LDR(bool debug, bool initialConnection) {
  if (initialConnection) {
    for (int i = 0; i < 10; i++) {
      int lightValue = analogRead(LDR_PIN);
      LDRFilter.calculate(lightValue);
      delay(1);
    }
  } else {
    int lightValue = analogRead(LDR_PIN);
    LDRFilter.calculate(lightValue);
  }
  bool lightPresent = LDRFilter.aboveThreshold();

  if (LDRDebounce.hasChanged(lightPresent) || initialConnection || LDRMessageTimer.isReady()) {
    client.publish(light_topic, (lightPresent) ? "light" : "dark");
  }

  if (debug) {
    Debug_Serial.print("The room is ");
    Debug_Serial.println((lightPresent) ? "bright" : "dark");
    Debug_Serial.print("Raw light reading: ");
    Debug_Serial.println(analogRead(LDR_PIN));
    Debug_Serial.print("Filtered light reading: ");
    Debug_Serial.print(LDRFilter.level);
    Debug_Serial.println("\n");
  }
}

void setup_LDR() {
  pinMode(LDR_PIN, INPUT);
  run_LDR(false, true);
  Debug_Serial.println("Photoresistor set up\n");
}

//LD2410C ----------------------------------------------------------------------------------------------------------------------------------------------------------
IntervalTimer LDTimer(100);
IntervalTimer LDDebugSerialTimer(1000);
IntervalTimer LDMessageTimer(MQTT_INTERVAL);
Debounce<bool> LDDebounce(false);
EMAFilter<float> LDFilter(0.25, 0.05, 0.4);
ld2410 motion_sensor;
HardwareSerial radarSerial(UART_INTERFACE);

void setup_LD(bool debug) {
  LD_Serial.begin(256000, SERIAL_8N1, LD_RX_PIN, LD_TX_PIN);
  motion_sensor.begin(LD_Serial);
  delay(1000);

  motion_sensor.read();
  if (motion_sensor.isConnected()) {
    Debug_Serial.println("LD2410 connected\n");
  } else {
    Debug_Serial.println("Could not connect to LD2410\n");
  }

  motion_sensor.setMaxValues(7, 7, 2);

  if (debug) {
    bool requestSuccess = motion_sensor.requestCurrentConfiguration();
    Debug_Serial.println((requestSuccess) ? "Configuration request a success" : "Couldn\'t request the configuration");
    if (requestSuccess) {
      Debug_Serial.print("Max gate: ");
      Debug_Serial.println(motion_sensor.max_gate);
      Debug_Serial.print("Max moving gate: ");
      Debug_Serial.println(motion_sensor.max_moving_gate);
      Debug_Serial.print("Max stationary gate: ");
      Debug_Serial.print(motion_sensor.max_stationary_gate);
      Debug_Serial.println("\n");
    }
  }
}

void run_LD(bool debug) {
  if (LDTimer.isReady()) {
    motion_sensor.read();
    bool rawPresence = (motion_sensor.stationaryTargetEnergy() > ENERGY_THRESHOLD || motion_sensor.movingTargetEnergy() > ENERGY_THRESHOLD);
    LDFilter.calculate((rawPresence) ? 1.0f : 0.0f);
    bool presence = LDFilter.aboveThreshold();
    bool debugOutputCondition = debug && LDDebugSerialTimer.isReady();

    if (LDDebounce.hasChanged(presence) || LDMessageTimer.isReady()) {
      client.publish(motion_topic, (presence) ? "ON" : "OFF");
    }
    
    if (motion_sensor.isConnected()) {
      if (debugOutputCondition) {
        Debug_Serial.println("---------------------------------");
        Debug_Serial.print("Presence: ");
        Debug_Serial.println((presence) ? "YES" : "NO");
        Debug_Serial.print("Raw presence boolean: ");
        Debug_Serial.println((rawPresence) ? "TRUE" : "FALSE");
        Debug_Serial.print("Filter level: ");
        Debug_Serial.println(LDFilter.level);
      }

      if (presence) {
        if (debugOutputCondition) {
          Debug_Serial.print("Distance: ");
          Debug_Serial.print((motion_sensor.stationaryTargetDetected()) ? motion_sensor.stationaryTargetDistance() : motion_sensor.movingTargetDistance());
          Debug_Serial.println("cm");
          Debug_Serial.print("Energy: ");
          Debug_Serial.print((motion_sensor.stationaryTargetDetected()) ? motion_sensor.stationaryTargetEnergy() : motion_sensor.movingTargetEnergy());
          Debug_Serial.println("\n");
        }
      } else {
        if (debugOutputCondition) {
          Debug_Serial.println("\n");
        }
      }
    } else {
      if (debugOutputCondition) {
        Debug_Serial.println("No data recieved\n");
      }
    }
  }
}