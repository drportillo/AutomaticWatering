#include "src/constants.h"
#include "src/sensors/sensors.h"
#include "src/screen/screen.h"
#include "src/network/wifi.h"
#include <WiFiS3.h>  // Include the WiFi library for the R4 WiFi


void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize the TFT display
  startScreen();

  //Connect to Wifi Network
  connectWifi(SECRET_SSID, SECRET_PASS);

  delay(500);
}

void loop() {
  // Array to store sensor data
  float temperatures[3];
  float humidities[3];

  bool wifi_status = WiFi.status() == WL_CONNECTED;
  bool blt_status = false;

  // Read temperature and humidity from all sensors
  temperatures[0] = readTemperature(NTC1_PIN);
  humidities[0] = readHumidity(HUMIDITY1_PIN);

  temperatures[1] = readTemperature(NTC2_PIN);
  humidities[1] = readHumidity(HUMIDITY2_PIN);

  temperatures[2] = readTemperature(NTC3_PIN);
  humidities[2] = readHumidity(HUMIDITY3_PIN);

  // Render the screen with the sensor data
  renderScreen(temperatures, humidities, wifi_status, blt_status);
}
