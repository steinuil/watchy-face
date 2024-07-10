#pragma once

// Data sheets:
// https://www.nxp.com/docs/en/data-sheet/PCF8563.pdf

#include <Wire.h>

namespace pcf8563 {

struct Time {
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
};

/** Month (1-12) */
enum Month : uint8_t {
  Unknown = 0,
  January = 1,
  February = 2,
  March = 3,
  April = 4,
  May = 5,
  June = 6,
  July = 7,
  August = 8,
  September = 9,
  October = 10,
  November = 11,
  December = 12,
};

/** Day of the week (Sunday=0-Saturday=6) */
enum Weekday : uint8_t {
  Sunday = 0,
  Monday = 1,
  Tuesday = 2,
  Wednesday = 3,
  Thursday = 4,
  Friday = 5,
  Saturday = 6,
};

// https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Sakamoto's_methods
static uint8_t weekday_offsets_by_month[12] = {0, 3, 2, 5, 0, 3,
                                               5, 1, 4, 6, 2, 4};
static Weekday calculate_weekday(uint32_t year, Month month, uint8_t day) {
  if (month < 3) {
    year -= 1;
  }

  uint8_t wday = (year + year / 4 - year / 100 + year / 400 +
                  weekday_offsets_by_month[month - 1] + day) %
                 7;
  return static_cast<Weekday>(wday);
}

class Date {
public:
  /** Full year (1990-2099) */
  uint32_t year;
  Month month;
  uint8_t day;
  Weekday weekday;

  Date()
      : year(2000), month(January), day(1),
        weekday(calculate_weekday(2000, January, 1)) {}
  Date(uint32_t year, Month month, uint8_t day)
      : year(year), month(month), day(day),
        weekday(calculate_weekday(year, month, day)) {}
};

class AlarmTime {
public:
  Weekday weekday;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;

  static const uint8_t DISABLE = 0x80;

  AlarmTime(uint8_t minute, uint8_t hour = DISABLE, uint8_t day = DISABLE,
            Weekday weekday = static_cast<Weekday>(DISABLE))
      : weekday(weekday), day(day), hour(hour), minute(minute) {}
};

enum Frequency {
  F_32_768KHz = 0b00,
  F_1024KHz = 0b01,
  F_32Hz = 0b10,
  F_1Hz = 0b11,
};

class Pcf8563 {
public:
  Pcf8563();

  void reset(void);
  void clear_control_status(void);

  void set_datetime(const Date &date, const Time &time);
  void read_datetime(Date &date, Time &time);

  void set_time(const Time &time);
  void read_time(Time &time);
  uint8_t read_minute();
  void set_date(const Date &date);
  void read_date(Date &date);

  /** Clear alarm flag and set alarm interrupt */
  void enable_alarm_interrupt(void);
  bool is_alarm_interrupt_enabled(void);
  bool is_alarm_active(void);

  void set_alarm(const AlarmTime &alarm);
  // void read_alarm(AlarmTime &alarm);
  /** Clear alarm flag but leave interrupt unchanged */
  // void reset_alarm(void);
  /** Clear alarm and interrupt */
  void clear_alarm(void);

  void set_square_wave(Frequency frequency);
  void disable_square_wave(void);

private:
  uint8_t read_control_status_1(void);
  uint8_t read_control_status_2(void);

  int address;
};

} // namespace pcf8563
