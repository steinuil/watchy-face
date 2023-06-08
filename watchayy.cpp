#include "watchayy.h"

#define LETTER_4X(char) passing_shot_letter_##char##_4x

static void digit_4x(uint8_t digit, const uint8_t **out)
{
  switch (digit)
  {
  case 0:
    *out = LETTER_4X(0);
    break;
  case 1:
    *out = LETTER_4X(1);
    break;
  case 2:
    *out = LETTER_4X(2);
    break;
  case 3:
    *out = LETTER_4X(3);
    break;
  case 4:
    *out = LETTER_4X(4);
    break;
  case 5:
    *out = LETTER_4X(5);
    break;
  case 6:
    *out = LETTER_4X(6);
    break;
  case 7:
    *out = LETTER_4X(7);
    break;
  case 8:
    *out = LETTER_4X(8);
    break;
  case 9:
    *out = LETTER_4X(9);
    break;
  }
}

void Watchayy::drawWatchFace()
{
  display.fillScreen(GxEPD_WHITE);

  const uint8_t *letter;

  digit_4x(currentTime.Hour / 10, &letter);
  display.drawBitmap(
      9 * 2,
      42 * 2,
      letter,
      8 * 4,
      8 * 4,
      GxEPD_BLACK);

  digit_4x(currentTime.Hour % 10, &letter);
  display.drawBitmap(
      27 * 2,
      42 * 2,
      letter,
      8 * 4,
      8 * 4,
      GxEPD_BLACK);

  digit_4x(currentTime.Hour % 10, &letter);
  display.drawBitmap(
      57 * 2,
      42 * 2,
      letter,
      8 * 4,
      8 * 4,
      GxEPD_BLACK);

  digit_4x(currentTime.Hour % 10, &letter);
  display.drawBitmap(
      75 * 2,
      42 * 2,
      letter,
      8 * 4,
      8 * 4,
      GxEPD_BLACK);
}
