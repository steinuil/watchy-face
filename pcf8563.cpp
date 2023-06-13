#include "pcf8563.h"

static const int RTCC_R = 0xA2;

static const uint8_t CONTROL_STATUS_1_ADDR = 0x00;
static const uint8_t CONTROL_STATUS_2_ADDR = 0x01;
static const uint8_t SECOND_ADDR = 0x02;
static const uint8_t MINUTE_ADDR = 0x03;
static const uint8_t DAY_ADDR = 0x05;
static const uint8_t ALARM_MINUTE_ADDR = 0x09;
static const uint8_t CLOCK_OUTPUT_ADDR = 0x0D;

/// Indicates whether the current century is 2000 (0) or 1900 (1)
static const uint8_t CENTURY_MASK = 0x80;
static const uint8_t ALARM_FLAG_MASK = 0x08;
static const uint8_t ALARM_INTERRUPT_ENABLED_MASK = 0x02;
static const uint8_t SQUARE_WAVE_ENABLED_MASK = 0b10000000;

static const uint8_t MONTH_MASK = 0b00011111;
static const uint8_t WEEKDAY_MASK = 0b00000111;
static const uint8_t DAY_MASK = 0b00111111;
static const uint8_t HOUR_MASK = 0b00111111;
static const uint8_t MINUTE_MASK = 0b01111111;
static const uint8_t SECOND_MASK = 0b01111111;

inline static uint8_t dec_to_bcd(uint8_t val) {
  return (val / 10 * 16) + (val % 10);
}
inline static uint8_t bcd_to_dec(uint8_t val) {
  return (val / 16 * 10) + (val % 16);
}

namespace pcf8563 {

Pcf8563::Pcf8563(void) {
  Wire.begin();
  address = RTCC_R >> 1;
}

void Pcf8563::reset(void) {
  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_1_ADDR);
  Wire.write(0x0);                // control/status 1
  Wire.write(0x0);                // control/status 2
  Wire.write(0x01);               // seconds
  Wire.write(0x01);               // minutes
  Wire.write(0x01);               // hour
  Wire.write(0x01);               // day
  Wire.write(0x01);               // weekday
  Wire.write(0x01);               // month + century
  Wire.write(0x01);               // year
  Wire.write(AlarmTime::DISABLE); // minute alarm value reset to 00
  Wire.write(AlarmTime::DISABLE); // hour alarm value reset to 00
  Wire.write(AlarmTime::DISABLE); // day alarm value reset to 00
  Wire.write(AlarmTime::DISABLE); // weekday alarm value reset to 00
  Wire.write(0x0);                // set SQW, see: setSquareWave
  Wire.write(0x0);                // timer off
  Wire.endTransmission();
}

void Pcf8563::clear_control_status(void) {
  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_1_ADDR);
  Wire.write(0x00); // Control/status 1
  Wire.write(0x00); // Control/status 2
  Wire.endTransmission();
}

uint8_t Pcf8563::read_control_status_1(void) {
  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_1_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  return Wire.read();
}

uint8_t Pcf8563::read_control_status_2(void) {
  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_2_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  return Wire.read();
}

void Pcf8563::set_datetime(const Date &date, const Time &time) {
  Wire.beginTransmission(address);

  Wire.write(SECOND_ADDR);
  Wire.write(dec_to_bcd(time.second));
  Wire.write(dec_to_bcd(time.minute));
  Wire.write(dec_to_bcd(time.hour));

  uint8_t month_bcd = dec_to_bcd(date.month);
  if (date.year < 2000) {
    month_bcd |= CENTURY_MASK;
  }

  uint8_t year = date.year % 100;

  Wire.write(dec_to_bcd(date.day));
  Wire.write(dec_to_bcd(date.weekday));
  Wire.write(month_bcd);
  Wire.write(dec_to_bcd(year));

  Wire.endTransmission();
}

void Pcf8563::read_datetime(Date &date, Time &time) {
  Wire.beginTransmission(address);
  Wire.write(SECOND_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 7);
  time.second = bcd_to_dec(Wire.read() & SECOND_MASK);
  time.minute = bcd_to_dec(Wire.read() & MINUTE_MASK);
  time.hour = bcd_to_dec(Wire.read() & HOUR_MASK);

  date.day = bcd_to_dec(Wire.read() & DAY_MASK);
  date.weekday = static_cast<Weekday>(bcd_to_dec(Wire.read() & WEEKDAY_MASK));

  uint8_t month_bcd = Wire.read();
  uint32_t year;
  if (month_bcd & CENTURY_MASK) {
    year = 1900;
  } else {
    year = 2000;
  }

  date.month = static_cast<Month>(bcd_to_dec(month_bcd & MONTH_MASK));
  date.year = year + bcd_to_dec(Wire.read());

  Wire.endTransmission();
}

void Pcf8563::set_time(const Time &time) {
  Wire.beginTransmission(address);
  Wire.write(SECOND_ADDR);
  Wire.write(dec_to_bcd(time.second));
  Wire.write(dec_to_bcd(time.minute));
  Wire.write(dec_to_bcd(time.hour));
  Wire.endTransmission();
}

void Pcf8563::set_date(const Date &date) {
  uint8_t month_bcd = dec_to_bcd(date.month);
  if (date.year < 2000) {
    month_bcd |= CENTURY_MASK;
  }

  uint8_t year = date.year % 100;

  Wire.beginTransmission(address);
  Wire.write(DAY_ADDR);
  Wire.write(dec_to_bcd(date.day));
  Wire.write(dec_to_bcd(date.weekday));
  Wire.write(month_bcd);
  Wire.write(dec_to_bcd(year));
  Wire.endTransmission();
}

void Pcf8563::read_time(Time &time) {
  Wire.beginTransmission(address);
  Wire.write(SECOND_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 3);
  time.second = bcd_to_dec(Wire.read() & SECOND_MASK);
  time.minute = bcd_to_dec(Wire.read() & MINUTE_MASK);
  time.hour = bcd_to_dec(Wire.read() & HOUR_MASK);
}

uint8_t Pcf8563::read_minute() {
  Wire.beginTransmission(address);
  Wire.write(MINUTE_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  return bcd_to_dec(Wire.read() & MINUTE_MASK);
}

void Pcf8563::read_date(Date &date) {
  Wire.beginTransmission(address);
  Wire.write(DAY_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(address, 4);
  date.day = bcd_to_dec(Wire.read() & DAY_MASK);
  date.weekday = static_cast<Weekday>(bcd_to_dec(Wire.read() & WEEKDAY_MASK));

  uint8_t month_bcd = Wire.read();
  uint32_t year;
  if (month_bcd & CENTURY_MASK) {
    year = 1900;
  } else {
    year = 2000;
  }

  date.month = static_cast<Month>(bcd_to_dec(month_bcd & MONTH_MASK));
  date.year = year + bcd_to_dec(Wire.read());
}

void Pcf8563::enable_alarm_interrupt(void) {
  uint8_t status_2 = read_control_status_2();
  status_2 &= ~ALARM_FLAG_MASK;
  status_2 |= ALARM_INTERRUPT_ENABLED_MASK;

  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_2_ADDR);
  Wire.write(status_2);
  Wire.endTransmission();
}

bool Pcf8563::is_alarm_interrupt_enabled(void) {
  uint8_t status_2 = read_control_status_2();
  return (status_2 & ALARM_INTERRUPT_ENABLED_MASK) > 0;
}

bool Pcf8563::is_alarm_active(void) {
  uint8_t status_2 = read_control_status_2();
  return (status_2 & ALARM_FLAG_MASK) > 0;
}

void Pcf8563::set_alarm(const AlarmTime &alarm) {
  Wire.beginTransmission(address);
  Wire.write(ALARM_MINUTE_ADDR);
  Wire.write(dec_to_bcd(alarm.minute));
  Wire.write(dec_to_bcd(alarm.hour));
  Wire.write(dec_to_bcd(alarm.day));
  Wire.write(dec_to_bcd(alarm.weekday));
  Wire.endTransmission();

  enable_alarm_interrupt();
}

void Pcf8563::clear_alarm(void) {
  uint8_t status_2 = read_control_status_2();
  status_2 &= ~ALARM_FLAG_MASK;
  status_2 &= ~ALARM_INTERRUPT_ENABLED_MASK;

  Wire.beginTransmission(address);
  Wire.write(CONTROL_STATUS_2_ADDR);
  Wire.write(status_2);
  Wire.endTransmission();
}

void Pcf8563::set_square_wave(Frequency frequency) {
  Wire.beginTransmission(address);
  Wire.write(CLOCK_OUTPUT_ADDR);
  Wire.write(frequency | SQUARE_WAVE_ENABLED_MASK);
  Wire.endTransmission();
}

void Pcf8563::disable_square_wave() {
  Wire.beginTransmission(address);
  Wire.write(CLOCK_OUTPUT_ADDR);
  Wire.write(0);
  Wire.endTransmission();
}

} // namespace pcf8563
