# Time Manager Module

## Overview

The `time_manager` module gives the Arduino UNO R4 WiFi awareness of the current date and time. It syncs once from an NTP (internet time) server as soon as WiFi connects, then uses the board's built-in hardware RTC to keep accurate time — even if the network drops.

## Files

| File | Purpose |
|---|---|
| `lib/time/time_manager.h` | Public API |
| `lib/time/time_manager.cpp` | NTP + RTC implementation |

## Configuration

Set your standard time (non-DST) offset in `include/constants.h`:

```cpp
#define TZ_BASE_OFFSET_HOURS 1  // Spain/Madrid: CET = UTC+1
```

DST is applied automatically — no manual changes needed when the clocks change.

## How It Works

1. `initTime()` is called in `setup()` — starts the RTC and NTP client.
2. On the first `loop()` iteration where WiFi is connected, `syncTimeNTP()` fetches UTC from `pool.ntp.org`, computes the correct local offset (CET or CEST), and sets the onboard hardware RTC.
3. The RTC (crystal-backed) keeps ticking accurately even if WiFi drops later.
4. Every 24 hours, `syncTimeNTP()` is called again automatically to correct any drift.

### DST auto-detection (EU rule)

The module implements the EU DST rule in software — no library required:

- **CEST (UTC+2):** starts on the last Sunday of March at 01:00 UTC
- **CET (UTC+1):** resumes on the last Sunday of October at 01:00 UTC

The correct offset is computed from the UTC epoch at sync time, so the RTC is always set to the right local time regardless of the time of year.

## API Reference

```cpp
void   initTime();       // Call once in setup()
bool   syncTimeNTP();    // Fetches time from NTP and sets the RTC; returns true on success
bool   isTimeSynced();   // Returns false until the first successful NTP sync

int    getHour();        // 0–23
int    getMinute();      // 0–59
int    getSecond();      // 0–59

String getFormattedTime();  // "HH:MM:SS"  e.g. "14:32:07"
String getFormattedDate();  // "YYYY-MM-DD" e.g. "2026-05-19"

// Returns true if the current time falls within [startHour:startMin, endHour:endMin).
// Handles ranges that wrap midnight (e.g. 22:00–06:00).
bool isTimeInRange(int startHour, int startMin, int endHour, int endMin);
```

### `isTimeInRange` examples

```cpp
// Water only between 7:00 AM and 7:00 PM
if (isTimeSynced() && isTimeInRange(7, 0, 19, 0)) {
    // run pump logic
}

// Active overnight: 10:00 PM to 6:00 AM
if (isTimeSynced() && isTimeInRange(22, 0, 6, 0)) {
    // midnight-spanning range works correctly
}
```

### Timestamping events

```cpp
Serial.println(getFormattedDate() + " " + getFormattedTime() + " Pump 1 ON");
// Output: "2026-05-19 14:32:07 Pump 1 ON"
```

## Dependencies

- `arduino-libraries/NTPClient@^3.2.1` (declared in `platformio.ini`)
- `RTC.h` — built into the Arduino UNO R4 core, no extra install needed
- `WiFiUdp.h` — bundled with the UNO R4 WiFi library
