#include "constants.h"
#include "passwords.h"
#include "sensors.h"
#include "screen.h"
#include "pumps.h"
#include "thingProperties.h"
#include "time_manager.h"

#ifndef SECRET_SSID
#define SECRET_SSID "Wifi SSID not defined. Define it in src/passwords.h"
#endif

#ifndef SECRET_PASS
#define SECRET_PASS "Wifi Password not defined. Define it in src/passwords.h"
#endif

static float    g_temperatures[3];
static float    g_humidities[3];
static bool     g_wifi_status = false;

// Re-sync NTP once every 24 hours
static const unsigned long NTP_RESYNC_INTERVAL_MS = 24UL * 60 * 60 * 1000;
static unsigned long       g_lastNtpSync = 0;

static void refreshScreen() {
  bool pumps[3] = {
    digitalRead(PUMP1) == HIGH,
    digitalRead(PUMP2) == HIGH,
    digitalRead(PUMP3) == HIGH
  };
  char timeStr[6] = "--:--";
  if (isTimeSynced()) {
    String t = getFormattedTime();  // "HH:MM:SS"
    strncpy(timeStr, t.c_str(), 5);
    timeStr[5] = '\0';
  }
  renderScreen(g_temperatures, g_humidities, pumps, g_wifi_status, false, timeStr);
}

void setup()
{
  Serial.begin(9600);

  initPumps();

  startScreen();

  initTime();

  initProperties();

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  delay(SETUP_DELAY_MS);
}

void loop()
{
  ArduinoCloud.update();

  g_wifi_status = ArduinoCloud.connected();

  if (g_wifi_status) {
    unsigned long now = millis();
    if (!isTimeSynced() || (now - g_lastNtpSync >= NTP_RESYNC_INTERVAL_MS)) {
      if (syncTimeNTP()) g_lastNtpSync = now;
    }
  }

  g_temperatures[0] = readTemperature(NTC1_PIN);
  g_humidities[0]   = readHumidity(HUMIDITY1_PIN);

  g_temperatures[1] = readTemperature(NTC2_PIN);
  g_humidities[1]   = readHumidity(HUMIDITY2_PIN);

  g_temperatures[2] = readTemperature(NTC3_PIN);
  g_humidities[2]   = readHumidity(HUMIDITY3_PIN);

  refreshScreen();

  char buffer[10];
  ac_t = "";
  ac_h = "";
  for (int i = 0; i < 3; i++) {
    ac_t = ac_t + dtostrf(g_temperatures[i], 6, 2, buffer);
    ac_h = ac_h + dtostrf(g_humidities[i], 6, 2, buffer);
  }

  delay(LOOP_DELAY_MS);
}

void onAcPump1Change() {
  setPump(PUMP1, ac_pump1);
  Serial.print("Pump 1: ");
  Serial.println(ac_pump1 ? "ON" : "OFF");
  refreshScreen();
}

void onAcPump2Change() {
  setPump(PUMP2, ac_pump2);
  Serial.print("Pump 2: ");
  Serial.println(ac_pump2 ? "ON" : "OFF");
  refreshScreen();
}

void onAcPump3Change() {
  setPump(PUMP3, ac_pump3);
  Serial.print("Pump 3: ");
  Serial.println(ac_pump3 ? "ON" : "OFF");
  refreshScreen();
}
