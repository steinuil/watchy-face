#pragma once

#include "pcf8563.h"
#include <TimeLib.h>

class WatchyRTC {
public:
  pcf8563::Pcf8563 rtc;

public:
  WatchyRTC();
  void set_next_minute_alarm();
  void read(tmElements_t &tm);
  void set(const tmElements_t& tm);
};
