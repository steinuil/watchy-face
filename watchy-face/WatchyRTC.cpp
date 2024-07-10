#include "WatchyRTC.h"

WatchyRTC::WatchyRTC() {}

/** Set alarm to trigger on the next minute. */
void WatchyRTC::set_next_minute_alarm() {
  uint8_t minute = rtc.read_minute();
  rtc.set_alarm(pcf8563::AlarmTime(minute >= 59 ? 0 : minute + 1));
}

void WatchyRTC::read(tmElements_t &tm) {
  auto date = pcf8563::Date{};
  auto time = pcf8563::Time{};

  rtc.read_datetime(date, time);
  tm.Year = date.year - 1970;
  tm.Month = date.month;
  tm.Day = date.day;
  tm.Wday = date.weekday + 1;
  tm.Hour = time.hour;
  tm.Minute = time.minute;
  tm.Second = time.second;
}

void WatchyRTC::set(const tmElements_t &tm) {
  rtc.set_datetime(pcf8563::Date(static_cast<uint32_t>(tm.Year) + 1970,
                                 static_cast<pcf8563::Month>(tm.Month), tm.Day),
                   pcf8563::Time{
                       tm.Hour,
                       tm.Minute,
                       tm.Second,
                   });

  set_next_minute_alarm();
}
