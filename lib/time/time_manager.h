#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

void   initTime();
bool   syncTimeNTP();
bool   isTimeSynced();

int    getHour();
int    getMinute();
int    getSecond();

String getFormattedTime();  // "HH:MM:SS"
String getFormattedDate();  // "YYYY-MM-DD"

// Returns true if current time falls within [startHour:startMin, endHour:endMin).
// Handles ranges that wrap midnight (e.g. 22:00 – 06:00).
bool isTimeInRange(int startHour, int startMin, int endHour, int endMin);

#endif
