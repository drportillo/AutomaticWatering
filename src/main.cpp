#include "constants.h"
#include "passwords.h"
#include "sensors.h"
#include "screen.h"
#include "thingProperties.h"
// #include "wifi.h"
// #include <WiFiS3.h> // Include the WiFi library for the R4 WiFi
#include <array>

#ifndef SECRET_SSID
#define SECRET_SSID "Wifi SSID not defined. Define it in src/passwords.h"
#endif

#ifndef SECRET_PASS
#define SECRET_PASS "Wifi Password not defined. Define it in src/passwords.h"
#endif

const int pumps[] = {PUMP1, PUMP2, PUMP3};

void setup()
{
  // Start serial communication
  Serial.begin(9600);

  pinMode(PUMP1, OUTPUT); // Set pin 2 as an output for water pump 1.
  pinMode(PUMP2, OUTPUT); // Set pin 3 as an output for water pump 2.
  pinMode(PUMP3, OUTPUT); // Set pin 4 as an output for water pump 3.

  // Initialize the TFT display
  startScreen();

  // Connect to Wifi Network
  // connectWifi(SECRET_SSID, SECRET_PASS);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  delay(3000);
}

void loop()
{
  ArduinoCloud.update();
  // Array to store sensor data
  float temperatures[3];
  float humidities[3];

  bool wifi_status = ArduinoCloud.connected(); //iFi.status() == WL_CONNECTED;
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

  char buffer[10]; // Buffer to hold the string
  ac_t = "";
  ac_h = "";
  for (int i = 0; i < 3; i++)
  {
    if (humidities[i] <= HUMIDITY_TRESHOLD_PUMP_ON)
    {
      digitalWrite(pumps[i], HIGH); // Turn on pump if humidity is low
    }
    else if (
        humidities[i] >= HUMIDITY_TRESHOLD_PUMP_OFF and digitalRead(pumps[i]) == HIGH) // turn off pump if humidity reached enough level.
    {
      digitalWrite(pumps[i], LOW);
    }
    // log to the cloud
    ac_t = ac_t + dtostrf(temperatures[i], 6, 2, buffer);
    ac_h = ac_h + dtostrf(humidities[i], 6, 2, buffer);
  }


  delay(5000);
}

/*
  Since AcPump1 is READ_WRITE variable, onAcPump1Change() is
  executed every time a new value is received from IoT Cloud.
*/
// void onAcPump1Change()
// {
//   // Add your code here to act upon AcPump1 change
// }

// /*
//   Since AcPump2 is READ_WRITE variable, onAcPump2Change() is
//   executed every time a new value is received from IoT Cloud.
// */
void onAcPump2Change()
{
  // Add your code here to act upon AcPump2 change
  Serial.println(ac_pump2);
  Serial.println("Bomba 2 activada");
  digitalWrite(PUMP2, ac_pump2);
}

// /*
//   Since AcPump is READ_WRITE variable, onAcPumpChange() is
//   executed every time a new value is received from IoT Cloud.
// */
// void onAcPump3Change()
// {
//   // Add your code here to act upon AcPump change
// }