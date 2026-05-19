# Network Module

## Overview

The `network` module is a minimal standalone WiFi connection helper. It is **not used in the current firmware** — WiFi is managed automatically by `ArduinoIoTCloud` via `WiFiConnectionHandler`. This module is kept for reference in case a standalone (non-cloud) connection is ever needed.

## Files

| File | Purpose |
|---|---|
| `lib/network/wifi.h` | Public API |
| `lib/network/wifi.cpp` | Blocking WiFi connect with serial status output |

## How It Works

`connectWifi()` calls `WiFi.begin()` and blocks in a loop, printing the connection status to Serial every second, until `WiFi.status() == WL_CONNECTED`. Once connected it prints the assigned IP address.

This is a simple blocking approach — suitable for a standalone sketch but incompatible with the non-blocking connection management that `ArduinoIoTCloud` requires.

## API Reference

```cpp
void connectWifi(char* ssid, char* pass);
// Blocks until connected. Prints status and IP to Serial at 9600 baud.
```

## Why It Is Unused

The current firmware uses `ArduinoIoTCloud` for remote monitoring and pump control. The cloud library manages its own `WiFiConnectionHandler` internally — calling `connectWifi()` alongside it would conflict. If the IoT Cloud integration is ever removed, this helper can be re-enabled to replace it.

## Dependencies

- `WiFiS3.h` — bundled with the Arduino UNO R4 WiFi core, no extra install needed
