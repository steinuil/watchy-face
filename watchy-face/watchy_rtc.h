#pragma once

#include "pcf8563.h"
#include <TimeLib.h>

namespace watchy::rtc {
void set_next_minute_alarm(pcf8563::Pcf8563 rtc);
void read(pcf8563::Pcf8563 rtc, tmElements_t &tm);
void set(pcf8563::Pcf8563 rtc, const tmElements_t &tm);
} // namespace watchy::rtc
