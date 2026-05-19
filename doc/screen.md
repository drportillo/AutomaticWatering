# Screen Module

## Overview

The `screen` module drives the 240×240 ST7789 TFT display. It renders a live dashboard showing one zone per column: an animated flower face, a temperature reading, and a humidity reading. A top bar shows WiFi and Bluetooth connectivity status.

## Files

| File | Purpose |
|---|---|
| `lib/screen/screen.h` | Public API |
| `lib/screen/screen.cpp` | Layout, drawing primitives, render logic |

## Configuration

The display is driven by `TFT_eSPI`, configured entirely via `build_flags` in `platformio.ini` — there is no `User_Setup.h`. SPI pins and driver settings:

```ini
-D ST7789_DRIVER=1
-D TFT_WIDTH=240  -D TFT_HEIGHT=240
-D TFT_MISO=11    -D TFT_MOSI=12
-D TFT_SCLK=13    -D TFT_CS=10
-D TFT_DC=9       -D TFT_RST=8
-D SPI_FREQUENCY=27000000
```

The disconnected-sensor threshold used by the renderer comes from `include/constants.h`:

```cpp
#define DISCONNECTED_TEMP_THRESHOLD 0
```

## Layout

The 240×240 screen is divided into a fixed grid:

```
y=0  ┌──────────────────────────────┐
     │  hh:mm     WiFi: ON  BT: OFF │  ← top bar (20px)
y=20 ├────────┬─────────┬───────────┤
     │        │         │           │
     │ zone 1 │  zone 2 │  zone 3   │  ← flower area (110px)
     │        │         │           │
y=130├────────┼─────────┼───────────┤
     │ TEMP   │  TEMP   │  TEMP     │  ← temperature row (55px)
     │ 22.5   │  23.1   │  ---      │
y=185├────────┼─────────┼───────────┤
     │  HUM   │   HUM   │   HUM     │  ← humidity row (55px)
     │  63%   │   47%   │   ---     │
y=240└────────┴─────────┴───────────┘
```

Each column is 80px wide.

## Flower Face

Each zone is represented by a flower whose face expression and colour reflect the current sensor state:

| Condition | Face | Colour |
|---|---|---|
| Humidity ≥ 60% | Happy (smile) | Temperature-dependent |
| Humidity 30–59% | Neutral (flat mouth) | Temperature-dependent |
| Humidity < 30% | Sad (frown) | Temperature-dependent |
| Sensor disconnected | Off (× eyes) | Temperature-dependent |

Face colour encodes temperature at a glance:

| Temperature | Colour |
|---|---|
| ≤ 10 °C | Blue |
| 11–25 °C | Yellow |
| > 25 °C | Red |

When a pump is actively running, three cyan water droplets are drawn above that zone's flower.

## Disconnected Sensors

If `temperatures[i] < DISCONNECTED_TEMP_THRESHOLD`, the zone is treated as disconnected: the flower shows × eyes, and both TEMP and HUM cells display `---` instead of a number.

## API Reference

```cpp
void startScreen();  // Call once in setup() — initialises TFT and draws placeholder UI

// Re-draws the entire screen with current readings.
// pumps[]: true = pump running (shows droplets)
// wifi_status / blt_status: true = connected (shown in green)
// timeStr: "HH:MM" shown in the top bar; pass "--:--" before NTP syncs
void renderScreen(float temperatures[], float humidities[], bool pumps[], bool wifi_status, bool blt_status, const char* timeStr);
```

### Usage

```cpp
// In setup()
startScreen();

// In loop() / after sensor reads
bool pumps[3] = {
    digitalRead(PUMP1) == HIGH,
    digitalRead(PUMP2) == HIGH,
    digitalRead(PUMP3) == HIGH
};
char timeStr[6] = "--:--";
if (isTimeSynced()) {
    String t = getFormattedTime();   // "HH:MM:SS"
    strncpy(timeStr, t.c_str(), 5);
    timeStr[5] = '\0';
}
renderScreen(g_temperatures, g_humidities, pumps, wifi_status, false, timeStr);
```

## Dependencies

- `bodmer/TFT_eSPI@^2.5.43` (declared in `platformio.ini`)
- `include/constants.h` — `DISCONNECTED_TEMP_THRESHOLD` and font/display constants
