#include "Arduino.h"
#include "pumps.h"
#include "constants.h"

static const int PUMP_PINS[] = {PUMP1, PUMP2, PUMP3};

void initPumps() {
  for (int i = 0; i < 3; i++) {
    pinMode(PUMP_PINS[i], OUTPUT);
    digitalWrite(PUMP_PINS[i], LOW);
  }
}

void updatePumps(float humidities[]) {
  for (int i = 0; i < 3; i++) {
    if (humidities[i] <= HUMIDITY_TRESHOLD_PUMP_ON) {
      digitalWrite(PUMP_PINS[i], HIGH);
    } else if (humidities[i] >= HUMIDITY_TRESHOLD_PUMP_OFF && digitalRead(PUMP_PINS[i]) == HIGH) {
      digitalWrite(PUMP_PINS[i], LOW);
    }
  }
}

void setPump(int pin, bool state) {
  digitalWrite(pin, state);
}
