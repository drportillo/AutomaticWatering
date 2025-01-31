#ifndef SENSORS_H
#define SENSORS_H

// Function to read temperature from an NTC thermistor
float readTemperature(int pin);

// Function to read humidity from a raw humidity sensor
float readHumidity(int pin);

#endif