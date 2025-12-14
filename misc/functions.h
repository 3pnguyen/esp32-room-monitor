#ifndef FUNCTIONS_H
#define FUNCTIONS_H

//ESP LED ----------------------------------------------------------------------------------------------------------------------------------------------------------

//blinks the on-board LED when the function begins and ends
void blink_built_in(bool onAfterwards, int speed = 500);

//WIFI ----------------------------------------------------------------------------------------------------------------------------------------------------------

//attempt to connect to WiFi. Trying to connect to WiFi for too long will timeout the function and it will not connect
void setup_WiFi(bool initialConnection);

//simply check if WiFi is connected, and if not, run the setup function to connect to WiFi. Runs in the main loop
void check_WiFi();

//MQTT ----------------------------------------------------------------------------------------------------------------------------------------------------------

void connect_MQTT(bool initialConnection);

//checks if MQTT is connected and handles the current connection. Will attempt to reconnect if it ever becomes disconnected
void run_MQTT();

//DHT ----------------------------------------------------------------------------------------------------------------------------------------------------------

//setup DHT pin-mode and start the DHT
void setup_DHT();

void run_DHT(bool debug = false);

//Photoresistor ----------------------------------------------------------------------------------------------------------------------------------------------------------

//gets the light value from the LDR, puts it into the filter, checks if the current filtered value is above the threshold, and sends to MQTT if state change
//if the function is being run initially, it will warm up the filter
void run_LDR(bool debug = false, bool initialConnection = false);

//sets the LDR pin-mode, and runs the "run_LDR" initially
void setup_LDR();

//LD2410C ----------------------------------------------------------------------------------------------------------------------------------------------------------

//setup the LD2410C and check if it is connected
void setup_LD(bool debug = false);

//get sensor data, filter it, and send, on an interval
void run_LD(bool debug = false);

#endif