#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define analog pins for NTC thermistors
#define NTC1_PIN A5
#define NTC2_PIN A4
#define NTC3_PIN A3

// Define analog pins for humidity sensors
#define HUMIDITY1_PIN A2
#define HUMIDITY2_PIN A1
#define HUMIDITY3_PIN A0

// Constants for NTC thermistor calculations
#define NTC_NOMINAL_RESISTANCE 10000  // Resistance at nominal temperature (usually 10kΩ)
#define NTC_NOMINAL_TEMPERATURE 25    // Nominal temperature (usually 25°C)
#define NTC_BETA_COEFFICIENT 3950     // Beta coefficient of the thermistor
#define SERIES_RESISTOR 10000         // Value of the fixed resistor in the voltage divider

// Constants for humidity sensor calibration
#define HUMIDITY_MIN 0       // Minimum analog value for humidity sensor
#define HUMIDITY_MAX 1023    // Maximum analog value for humidity sensor
#define HUMIDITY_RH_MIN 0    // Minimum relative humidity (%)
#define HUMIDITY_RH_MAX 100  // Maximum relative humidity (%)

// Threshold to determine if a sensor is disconnected
#define DISCONNECTED_TEMP_THRESHOLD 0

// Circle positions for visual indicators
#define CIRCLE_RADIUS 5
#define CIRCLE_SPACING 10
#define CIRCLE_Y 10
#define CIRCLE_X_START 190

#define SECRET_SSID "CasaRP"
#define SECRET_PASS "Zapati!!a1"

#endif