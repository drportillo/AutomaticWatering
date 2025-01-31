#include "Arduino.h"
#include "sensors.h"
#include "../constants.h"

// Function to read temperature from an NTC thermistor
float readTemperature(int pin) {
  // Read the analog value from the thermistor
  int analogValue = analogRead(pin);

  // Convert the analog value to resistance
  float resistance = SERIES_RESISTOR / ((1023.0 / analogValue) - 1);

  // Calculate temperature using the Beta parameter equation
  float temperature = 1.0 / (1.0 / (NTC_NOMINAL_TEMPERATURE + 273.15) + (1.0 / NTC_BETA_COEFFICIENT) * log(resistance / NTC_NOMINAL_RESISTANCE));
  temperature -= 273.15; // Convert from Kelvin to Celsius

  return temperature;
}

// Function to read humidity from a raw humidity sensor
float readHumidity(int pin) {
  // Read the analog value from the humidity sensor
  int analogValue = analogRead(pin);

  // Map the analog value to a humidity percentage (calibration may be required)
  float humidity = map(analogValue, HUMIDITY_MIN, HUMIDITY_MAX, HUMIDITY_RH_MIN, HUMIDITY_RH_MAX);

  return humidity;
}