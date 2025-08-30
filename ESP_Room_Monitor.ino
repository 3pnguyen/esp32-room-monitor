//imports ----------------------------------------------------------------------------------------------------------------------------------------------------------
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h> //needs to be installed from the library manager
//for DHT
#include <DHT.h> //needs to be installed from the library manager
#include <cmath>
//for LD2410
#include <ld2410.h> //needs to be installed from the library manager
#include <HardwareSerial.h>
//for customizable information
#include "sensitive_info.h"
#include "not_sensitive_info.h"

//classes ----------------------------------------------------------------------------------------------------------------------------------------------------------
//nonblocking timer that tracks the time between intervals
class IntervalTimer {
  private:
    unsigned long lastTime = 0;
    unsigned long interval;

  public:
    IntervalTimer(unsigned long intervalMs) : interval(intervalMs) {}

    bool isReady() {
      unsigned long currentTime = millis();

      if (currentTime - lastTime >= interval) {
        lastTime = currentTime;
        return true;
      }
      return false;
    }

    void reset() {
      lastTime = millis();
    }
};

//simple, on-off, true-false, state-detection class. Used to send light and motion info through MQTT only when they change state
template <typename T>
class Debounce {
  public:
    T firstValue;
    T secondValue;

    Debounce(T initialState) {
      firstValue = initialState;
      secondValue = initialState;
    }

    Debounce(T initialState, T secondState) {
      firstValue = initialState;
      secondValue = secondState;
    }

    bool hasChanged(T currentState) {
      if (currentState == firstValue && currentState == secondValue) {
        return false;
      }

      secondValue = firstValue;
      firstValue = currentState;
      return true;
    }

    bool lastState(T state) {
      return (state == firstValue);
    }
};

//digital filter to reduce sensor noise
template <typename T>
class EMAFilter {
  public:
    T level = T{};
    const float alpha;
    const float alpha2;
    const T threshold;

    EMAFilter(float smoothingFactor, T thresholdValue) : alpha(smoothingFactor), alpha2(smoothingFactor), threshold(thresholdValue) {}

    EMAFilter(float alphaHigh, float alphaLow, T thresholdValue) : alpha(alphaHigh), alpha2(alphaLow), threshold(thresholdValue) {}

    void calculate(T input) {
      if (alpha == alpha2) {
        level = (input * alpha) + (level * (1 - alpha));
      } else {
        float selectedAlpha = (input > level) ? alpha : alpha2;
        level = (input * selectedAlpha) + (level * (1 - selectedAlpha));
      }
    }

    void reset() {
      level = T{};
    }

    bool aboveThreshold () {
      return level > threshold;
    }
};

//Serial ----------------------------------------------------------------------------------------------------------------------------------------------------------
#define Debug_Serial Serial //renames the Serial into more descriptive names
#define LD_Serial Serial2

//ESP LED ----------------------------------------------------------------------------------------------------------------------------------------------------------
void blink_built_in(bool onAfterwards, int speed = 500) { //blinks the on-board LED when the function begins and ends
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

//WiFi ----------------------------------------------------------------------------------------------------------------------------------------------------------
IntervalTimer WiFiTimer(10000);

void setup_WiFi(bool initialConnection) { //attempt to connect to WiFi. Trying to connect to WiFi for too long will timeout the function and it will not connect
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

void check_WiFi() { //simply check if WiFi is connected, and if not, run the setup function to connect to WiFi. Runs in the main loop
  if(WiFi.status() != WL_CONNECTED) {
    setup_WiFi(false);
  }
}

//MQTT ----------------------------------------------------------------------------------------------------------------------------------------------------------
const char* motion_topic = "room/motion";
const char* temperature_topic = "room/temperature";
const char* humidity_topic = "room/humidity";
const char* light_topic = "room/light";

WiFiClient espClient; //mandatory setup for connecting to MQTT
PubSubClient client(espClient);

void connect_MQTT() { //attempts to connect to the MQTT server
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

void run_MQTT() { //checks if MQTT is connected and handles the current connection. Will attempt to reconnect if it ever becomes disconnected
  if(!client.connected()) {
    connect_MQTT();
  }
  client.loop();
}

//DHT ----------------------------------------------------------------------------------------------------------------------------------------------------------
IntervalTimer DHTTimer(2000);
DHT dht(DHT_PIN, DHT_TYPE);

void setup_DHT() { //setup DHT pin-mode and start the DHT
  pinMode(DHT_PIN, INPUT);

  dht.begin();
  delay(2000);

  Debug_Serial.println("DHT11 set up\n");
}

void run_DHT(bool debug = false) { //if enough time has passed, check the DHT readings and send through MQTT
  if (DHTTimer.isReady()) {
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
Debounce<bool> LDRDebounce(false);
EMAFilter<int> LDRFilter(0.2, 0.1, LIGHT_THRESHOLD);

//gets the light value from the LDR, puts it into the filter, checks if the current filtered value is above the threshold, and sends to MQTT if state change
//if the function is being run initially, it will warm up the filter
void run_LDR(bool debug = false, bool initialConnection = false) {
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

  if (LDRDebounce.hasChanged(lightPresent) || initialConnection) {
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

void setup_LDR() { //sets the LDR pin-mode, and runs the "run_LDR" initially
  pinMode(LDR_PIN, INPUT);
  run_LDR(false, true);
  Debug_Serial.println("Photoresistor set up\n");
}

//LD2410C ----------------------------------------------------------------------------------------------------------------------------------------------------------
IntervalTimer LDTimer(100);
IntervalTimer LDDebugSerialTimer(1000);
Debounce<bool> LDDebounce(false);
EMAFilter<float> LDFilter(0.25, 0.05, 0.4);
ld2410 motion_sensor;
HardwareSerial radarSerial(UART_INTERFACE);

void setup_LD(bool debug = false) { //setup the LD2410C and check if it is connected
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

void run_LD(bool debug = false) { //get sensor data, filter it, and send, on an interval
  if (LDTimer.isReady()) {
    motion_sensor.read();
    bool rawPresence = (motion_sensor.stationaryTargetEnergy() > ENERGY_THRESHOLD || motion_sensor.movingTargetEnergy() > ENERGY_THRESHOLD);
    LDFilter.calculate((rawPresence) ? 1.0f : 0.0f);
    bool presence = LDFilter.aboveThreshold();
    bool debugOutputCondition = debug && LDDebugSerialTimer.isReady();

    if (LDDebounce.hasChanged(presence)) {
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

//----------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {//sets up the debug serial monitor, WiFi, OTA, MQTT, and the other sensors
  blink_built_in(true);

  //debug serial monitor
  Debug_Serial.begin(115200);

  //WiFi
  setup_WiFi(true);

  //OTA
  ArduinoOTA.setPassword(OTA_PASS);
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.begin();

  //MQTT
  client.setServer(MQTT_SERVER_IP, MQTT_PORT);
  connect_MQTT();

  //sensors
  setup_LDR();
  setup_LD();
  setup_DHT();

  blink_built_in(false);
}

void loop() { //maintains connections and runs sensors
  delay(20); //delay to make the loop smoother
  
  //for maintaining WiFi, OTA, and MQTT
  ArduinoOTA.handle();
  run_MQTT();
  check_WiFi();

  //run the main sensors and send data
  run_DHT();
  run_LDR();
  run_LD();
}
