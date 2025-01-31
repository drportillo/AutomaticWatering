#include "Arduino.h"
#include "wifi.h"
#include "WiFiS3.h"
#include "/home/deck/Arduino/AutomaticWatering/src/constants.h"

void connectWifi(char* ssid, char* password) {
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(WiFi.status());
  }

  // Once connected, print the IP address
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}