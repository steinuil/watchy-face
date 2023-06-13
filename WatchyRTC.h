#pragma once

#include "config.h"
#include "time.h"
#include "Rtc_Pcf8563.h"
#include <TimeLib.h>

#define RTC_PCF_ADDR 0x51
#define YEAR_OFFSET_PCF 2000

class WatchyRTC {
public:
  Rtc_Pcf8563 rtc_pcf;

public:
  WatchyRTC();
  void init();
  void config(String datetime); // String datetime format is YYYY:MM:DD:HH:MM:SS
  void clearAlarm();
  void read(tmElements_t &tm);
  void set(tmElements_t tm);
  uint8_t temperature();

private:
  void _DSConfig(String datetime);
  void _PCFConfig(String datetime);
  int _getDayOfWeek(int d, int m, int y);
  String _getValue(String data, char separator, int index);
};
