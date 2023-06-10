#include "watchayy.h"

void Watchayy::drawWatchFace() {
  display.fillScreen(GxEPD_WHITE);

  // Time
  drawNumber(currentTime.Hour, 2, 9 * 2, 42 * 2, SCALE_4X, JUSTIFY_LEFT);
  display.drawBitmap(47 * 2, 43 * 2, passing_shot_colon_4x, 6 * 2, 14 * 2,
                     GxEPD_BLACK);
  drawNumber(currentTime.Minute, 2, 57 * 2, 42 * 2, SCALE_4X, JUSTIFY_LEFT);

  // Date
  drawNumber(currentTime.Day, 2, 4 * 2, 16 * 2, SCALE_2X, JUSTIFY_LEFT);
  drawString(weekdays[currentTime.Wday], 25 * 2, 16 * 2, SCALE_2X);
  drawString(months[currentTime.Month], 4 * 2, 28 * 2, SCALE_2X);

  // Step counter
  if (currentTime.Hour == 0 && currentTime.Minute == 0) {
    sensor.resetStepCounter();
  }
  drawString("STEPS", 52 * 2, 8, SCALE_2X);
  drawNumber(sensor.getCounter(), 1, 48 * 2, 8, SCALE_2X, JUSTIFY_RIGHT);

  // Temperature
  drawNumber(sensor.readTemperature(), 1, 82 * 2, 28 * 2, SCALE_2X,
             JUSTIFY_RIGHT);
  display.drawBitmap(83 * 2, 28 * 2, passing_shot_square_2x, 8, 8, GxEPD_BLACK);
  drawString("C", 88 * 2, 28 * 2, SCALE_2X);
}

void Watchayy::drawString(const char *str, uint16_t x, uint16_t y,
                          FONT_SCALE scale) {
  uint16_t size = 8 * scale;
  uint16_t spacing = scale;
  const uint8_t *const *letters;

  switch (scale) {
  case SCALE_1X:
    letters = letters_1x;
    break;
  case SCALE_2X:
    letters = letters_2x;
    break;
  case SCALE_4X:
    letters = letters_4x;
    break;
  }

  for (size_t i = 0; str[i] != '\0'; i++) {
    char l = str[i] - 'A';
    display.drawBitmap(x + (size + spacing) * i, y, letters[l], size, size,
                       GxEPD_BLACK);
  }
}

// TODO negative numbers
void Watchayy::drawNumber(int64_t num, uint8_t padding, uint16_t start_x,
                          uint16_t y, FONT_SCALE scale, JUSTIFICATION just) {
  uint16_t size = 8 * scale;
  uint16_t spacing = scale;
  const uint8_t *const *digits;

  switch (scale) {
  case SCALE_1X:
    digits = digits_1x;
    break;
  case SCALE_2X:
    digits = digits_2x;
    break;
  case SCALE_4X:
    digits = digits_4x;
    break;
  }

  if (num == 0) {
    for (size_t i = 0; i < padding; i++) {
      uint16_t x = just == JUSTIFY_LEFT ? start_x : start_x - size;
      display.drawBitmap(x, y, digits[0], size, size, GxEPD_BLACK);
      start_x += size + spacing;
    }
    return;
  }

  bool is_negative = num < 0;
  int32_t num_abs = is_negative ? -num : num;

  uint32_t digit_mask = 1;
  uint8_t digit_count = 0;
  for (size_t n = num_abs; n != 0; n /= 10) {
    digit_count += 1;
    digit_mask *= 10;
  }

  if (padding > digit_count) {
    padding -= digit_count;
  } else {
    padding = 0;
  }

  int8_t total_digits = digit_count + padding;
  // The number of pixels to advance start_x by every time we print a number
  int8_t advance_x = size + spacing;

  if (just == JUSTIFY_RIGHT) {
    start_x -= size * total_digits + spacing * (total_digits - 1);
  }

  if (is_negative == true) {
    uint16_t minus_width = 6 * scale;

    uint16_t x =
        just == JUSTIFY_LEFT ? start_x : start_x - minus_width - spacing;

    display.drawBitmap(x, y + (3 * scale), digits[10], minus_width, 3 * scale,
                       GxEPD_BLACK);

    if (just == JUSTIFY_LEFT) {
      start_x += minus_width + spacing;
    }
  }

  for (uint8_t i = 0; i < padding; i++) {
    display.drawBitmap(start_x, y, digits[0], size, size, GxEPD_BLACK);
    start_x += advance_x;
  }

  for (size_t i = 0; digit_mask > 1; digit_mask /= 10, i++) {
    uint16_t d = (num_abs % digit_mask) * 10 / digit_mask;
    display.drawBitmap(start_x, y, digits[d], size, size, GxEPD_BLACK);
    start_x += advance_x;
  }
}
