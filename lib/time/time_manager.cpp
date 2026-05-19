#include "time_manager.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <RTC.h>
#include <time.h>
#include "constants.h"

static WiFiUDP    _udp;
static NTPClient  _ntp(_udp, "pool.ntp.org");
static bool       _synced = false;

// Returns the day-of-month of the last Sunday in the given month/year.
static int _lastSundayOfMonth(int year, int month) {
    // Last day of month
    int lastDay;
    if (month == 2)
        lastDay = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11)
        lastDay = 30;
    else
        lastDay = 31;

    // Day-of-week for the last day (Tomohiko Sakamoto's algorithm, 0=Sunday)
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = year - (month < 3 ? 1 : 0);
    int dow = (y + y/4 - y/100 + y/400 + t[month - 1] + lastDay) % 7;

    return lastDay - dow;
}

// EU DST rule: CEST starts last Sunday of March at 01:00 UTC,
//              ends   last Sunday of October at 01:00 UTC.
static bool _isCEST(time_t utcEpoch) {
    struct tm *t = gmtime(&utcEpoch);
    int year  = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day   = t->tm_mday;
    int hour  = t->tm_hour;

    if (month < 3 || month > 10) return false;
    if (month > 3 && month < 10) return true;

    int lastSun = _lastSundayOfMonth(year, month);

    if (month == 3)
        return (day > lastSun) || (day == lastSun && hour >= 1);

    // month == 10
    return (day < lastSun) || (day == lastSun && hour < 1);
}

static void _applyEpochToRTC(unsigned long epoch) {
    int dstSeconds  = _isCEST((time_t)epoch) ? 3600 : 0;
    time_t local    = (time_t)(epoch + (long)TZ_BASE_OFFSET_HOURS * 3600L + dstSeconds);
    struct tm *ti   = gmtime(&local);
    RTCTime rtcTime(
        ti->tm_mday,
        static_cast<Month>(ti->tm_mon + 1),
        ti->tm_year + 1900,
        ti->tm_hour,
        ti->tm_min,
        ti->tm_sec,
        static_cast<DayOfWeek>(ti->tm_wday),
        SaveLight::SAVING_TIME_INACTIVE
    );
    RTC.setTime(rtcTime);
}

void initTime() {
    RTC.begin();
    _ntp.begin();
}

bool syncTimeNTP() {
    if (!_ntp.forceUpdate()) return false;
    _applyEpochToRTC(_ntp.getEpochTime());
    _synced = true;
    return true;
}

bool isTimeSynced() {
    return _synced;
}

static RTCTime _now() {
    RTCTime t;
    RTC.getTime(t);
    return t;
}

int getHour()   { return _now().getHour(); }
int getMinute() { return _now().getMinutes(); }
int getSecond() { return _now().getSeconds(); }

String getFormattedTime() {
    RTCTime t = _now();
    char buf[9];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t.getHour(), t.getMinutes(), t.getSeconds());
    return String(buf);
}

String getFormattedDate() {
    RTCTime t = _now();
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", t.getYear(), (int)t.getMonth(), t.getDayOfMonth());
    return String(buf);
}

bool isTimeInRange(int startHour, int startMin, int endHour, int endMin) {
    int current = getHour() * 60 + getMinute();
    int start   = startHour * 60 + startMin;
    int end     = endHour   * 60 + endMin;
    if (start <= end)
        return current >= start && current < end;
    // Wraps midnight
    return current >= start || current < end;
}
