#include "Arduino.h"
#include "wifi.h"
#include "WiFiS3.h"
#include "../../src/constants.h"

void connectWifi(char* ssid, char* password) {
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(WiFi.status());
  }

  // Once connected, print the IP address
  Serial.print("\nConnected to ");
  Serial.print(ssid);
  Serial.println("!");
  Serial.print("\tIP Address: ");
  Serial.println(WiFi.localIP());
}