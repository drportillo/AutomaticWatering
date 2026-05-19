# Sensors Module

## Overview

The `sensors` module reads temperature and soil moisture for all three watering zones. Temperature is measured with NTC thermistors using the Beta parameter equation. Soil moisture is read from resistive humidity sensors and mapped to a 0–100% scale.

## Files

| File | Purpose |
|---|---|
| `lib/sensors/sensors.h` | Public API |
| `lib/sensors/sensors.cpp` | Beta equation and humidity mapping |

## Configuration

All sensor parameters live in `include/constants.h`:

```cpp
// Pin assignments
#define NTC1_PIN A5
#define NTC2_PIN A4
#define NTC3_PIN A3
#define HUMIDITY1_PIN A2
#define HUMIDITY2_PIN A1
#define HUMIDITY3_PIN A0

// NTC thermistor parameters
#define NTC_NOMINAL_RESISTANCE  10000  // Ω at nominal temperature
#define NTC_NOMINAL_TEMPERATURE 25     // °C
#define NTC_BETA_COEFFICIENT    3950
#define SERIES_RESISTOR         10000  // fixed resistor in voltage divider

// Humidity sensor calibration
#define HUMIDITY_MIN  1022   // analog value when sensor is dry (in air)
#define HUMIDITY_MAX   300   // analog value when sensor is fully wet
#define HUMIDITY_RH_MIN  0   // mapped output minimum (%)
#define HUMIDITY_RH_MAX 100  // mapped output maximum (%)

// Below this temperature a sensor is considered disconnected
#define DISCONNECTED_TEMP_THRESHOLD 0
```

## How It Works

### Temperature — Beta equation

The thermistor and a fixed 10 kΩ resistor form a voltage divider powered by the 3.3 V rail. The ADC reads the voltage across the thermistor, which is converted to resistance and then to temperature:

```
R = SERIES_RESISTOR / (1023 / ADC - 1)

1/T = 1/T₀ + (1/β) × ln(R / R₀)
```

where T₀ = 298.15 K (25 °C), R₀ = 10 kΩ, β = 3950. The result is converted from Kelvin to Celsius.

A reading below `DISCONNECTED_TEMP_THRESHOLD` (0 °C) means no sensor is connected — the screen module uses this to display `---` instead of a number.

### Humidity — linear mapping

The resistive moisture sensor outputs a high analog value in dry soil and a lower value in wet soil. `readHumidity()` maps the raw ADC reading linearly onto 0–100% using Arduino's `map()`:

```
humidity% = map(ADC, HUMIDITY_MIN, HUMIDITY_MAX, 0, 100)
```

`HUMIDITY_MIN` and `HUMIDITY_MAX` may need recalibration for your specific sensors — measure the raw value in dry air and submerged in water respectively and update `constants.h`.

## API Reference

```cpp
float readTemperature(int pin);  // Returns °C; negative/near-zero means disconnected
float readHumidity(int pin);     // Returns 0–100 %RH
```

### Usage

```cpp
float temp = readTemperature(NTC1_PIN);   // zone 1 temperature
float hum  = readHumidity(HUMIDITY1_PIN); // zone 1 moisture

if (temp < DISCONNECTED_TEMP_THRESHOLD) {
    // sensor not connected
}
```

## Dependencies

- `Arduino.h` — `analogRead()`, `map()`
- `include/constants.h` — pin mappings and calibration values
- No external libraries required
