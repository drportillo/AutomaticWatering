# Pumps Module

## Overview

The `pumps` module controls three relay-driven water pumps, one per zone. It handles pin initialisation, threshold-based automatic control, and manual overrides from the Arduino IoT Cloud dashboard.

## Files

| File | Purpose |
|---|---|
| `lib/pumps/pumps.h` | Public API |
| `lib/pumps/pumps.cpp` | Pin setup, threshold logic, manual control |

## Configuration

Pump pins and humidity thresholds are defined in `include/constants.h`:

```cpp
#define PUMP1 D2
#define PUMP2 D3
#define PUMP3 D4

#define HUMIDITY_TRESHOLD_PUMP_ON  50   // turn ON  when humidity falls below this (%)
#define HUMIDITY_TRESHOLD_PUMP_OFF 85   // turn OFF when humidity rises above this (%)
```

The ON/OFF thresholds form a hysteresis band — the pump turns on at 50% and only stops at 85%, avoiding rapid on/off cycling.

## How It Works

### Initialisation

`initPumps()` sets all three pump pins as outputs and drives them LOW (off). This ensures no pump is running on startup, even briefly, before any sensor readings are taken.

### Automatic control — `updatePumps()`

Called with the latest humidity readings for all three zones. For each zone independently:

- If humidity ≤ `HUMIDITY_TRESHOLD_PUMP_ON` → drive pin HIGH (pump on)
- If humidity ≥ `HUMIDITY_TRESHOLD_PUMP_OFF` **and** the pump is currently running → drive pin LOW (pump off)

The second condition (`digitalRead` check) prevents needlessly re-writing LOW to a pin that is already off.

> **Note:** `updatePumps()` is currently not called from `main.cpp` — automatic pump control is commented out while capacitive sensor calibration is in progress. Manual control via `setPump()` is active.

### Manual control — `setPump()`

Used by the Arduino IoT Cloud callbacks in `main.cpp`. When the user toggles a pump on the cloud dashboard, `setPump(pin, state)` immediately writes the requested state to the relay pin.

## API Reference

```cpp
void initPumps();                      // Call once in setup()
void updatePumps(float humidities[]);  // Automatic threshold-based control (all 3 zones)
void setPump(int pin, bool state);     // Manual override for a single pump
```

### Usage

```cpp
// Automatic (currently disabled, pending sensor calibration)
updatePumps(g_humidities);

// Manual override from IoT Cloud callback
void onAcPump1Change() {
    setPump(PUMP1, ac_pump1);
}
```

## Dependencies

- `Arduino.h` — `pinMode()`, `digitalWrite()`, `digitalRead()`
- `include/constants.h` — pin mappings and thresholds
- No external libraries required
