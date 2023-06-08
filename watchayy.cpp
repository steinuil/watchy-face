#include "watchayy.h"

#define LETTER_4X(char) passing_shot_letter_##char##_4x
#define LETTER_2X(char) passing_shot_letter_##char##_2x
#define LETTER_1X(char) passing_shot_letter_##char

const uint8_t *const digits_4x[] = {
    LETTER_4X(0), LETTER_4X(1), LETTER_4X(2), LETTER_4X(3), LETTER_4X(4),
    LETTER_4X(5), LETTER_4X(6), LETTER_4X(7), LETTER_4X(8), LETTER_4X(9),
};

const uint8_t *const digits_2x[] = {
    LETTER_2X(0), LETTER_2X(1), LETTER_2X(2), LETTER_2X(3), LETTER_2X(4),
    LETTER_2X(5), LETTER_2X(6), LETTER_2X(7), LETTER_2X(8), LETTER_2X(9),
};

const uint8_t *const digits_1x[] = {
    LETTER_1X(0), LETTER_1X(1), LETTER_1X(2), LETTER_1X(3), LETTER_1X(4),
    LETTER_1X(5), LETTER_1X(6), LETTER_1X(7), LETTER_1X(8), LETTER_1X(9),
};

const uint8_t *const letters_4x[] = {
    LETTER_4X(A), LETTER_4X(B), LETTER_4X(C), LETTER_4X(D), LETTER_4X(E),
    LETTER_4X(F), LETTER_4X(G), LETTER_4X(H), LETTER_4X(I), LETTER_4X(J),
    LETTER_4X(K), LETTER_4X(L), LETTER_4X(M), LETTER_4X(N), LETTER_4X(O),
    LETTER_4X(P), LETTER_4X(Q), LETTER_4X(R), LETTER_4X(S), LETTER_4X(T),
    LETTER_4X(U), LETTER_4X(V), LETTER_4X(W), LETTER_4X(X), LETTER_4X(Y),
    LETTER_4X(Z),
};

const uint8_t *const letters_2x[] = {
    LETTER_2X(A), LETTER_2X(B), LETTER_2X(C), LETTER_2X(D), LETTER_2X(E),
    LETTER_2X(F), LETTER_2X(G), LETTER_2X(H), LETTER_2X(I), LETTER_2X(J),
    LETTER_2X(K), LETTER_2X(L), LETTER_2X(M), LETTER_2X(N), LETTER_2X(O),
    LETTER_2X(P), LETTER_2X(Q), LETTER_2X(R), LETTER_2X(S), LETTER_2X(T),
    LETTER_2X(U), LETTER_2X(V), LETTER_2X(W), LETTER_2X(X), LETTER_2X(Y),
    LETTER_2X(Z),
};

const uint8_t *const letters_1x[] = {
    LETTER_1X(A), LETTER_1X(B), LETTER_1X(C), LETTER_1X(D), LETTER_1X(E),
    LETTER_1X(F), LETTER_1X(G), LETTER_1X(H), LETTER_1X(I), LETTER_1X(J),
    LETTER_1X(K), LETTER_1X(L), LETTER_1X(M), LETTER_1X(N), LETTER_1X(O),
    LETTER_1X(P), LETTER_1X(Q), LETTER_1X(R), LETTER_1X(S), LETTER_1X(T),
    LETTER_1X(U), LETTER_1X(V), LETTER_1X(W), LETTER_1X(X), LETTER_1X(Y),
    LETTER_1X(Z),
};

void Watchayy::drawWatchFace() {
  display.fillScreen(GxEPD_WHITE);

  drawNumber(currentTime.Hour, 2, 9 * 2, 42 * 2, SCALE_4X, JUSTIFY_LEFT);
  drawNumber(currentTime.Minute, 2, 57 * 2, 42 * 2, SCALE_4X, JUSTIFY_LEFT);

  drawNumber(currentTime.Day, 2, 8 * 2, 16 * 2, SCALE_2X, JUSTIFY_LEFT);

  drawString("STEPS", 52 * 2, 8, SCALE_2X);
  drawNumber(sensor.getCounter(), 1, 44 * 2, 8, SCALE_2X, JUSTIFY_RIGHT);
}

void Watchayy::drawString(const char *str, uint16_t x, uint16_t y,
                          FONT_SCALE scale) {
  uint16_t size;
  uint16_t spacing;
  const uint8_t *const *letters;

  switch (scale) {
  case SCALE_1X:
    size = 8;
    spacing = 1;
    letters = letters_1x;
    break;
  case SCALE_2X:
    size = 16;
    spacing = 2;
    letters = letters_2x;
    break;
  case SCALE_4X:
    size = 32;
    spacing = 4;
    letters = letters_4x;
    break;
  }

  for (size_t i = 0; str[i] != '\0'; i++) {
    char l = str[i] - 'A';
    display.drawBitmap(x + (size + spacing) * i, y, letters[l], size, size,
                       GxEPD_BLACK);
  }
}

void Watchayy::drawNumber(uint64_t num, uint8_t padding, uint16_t start_x,
                          uint16_t y, FONT_SCALE scale, JUSTIFICATION just) {
  uint16_t size;
  uint16_t spacing;
  const uint8_t *const *digits;

  switch (scale) {
  case SCALE_1X:
    size = 8;
    spacing = 1;
    digits = digits_1x;
    break;
  case SCALE_2X:
    size = 16;
    spacing = 2;
    digits = digits_2x;
    break;
  case SCALE_4X:
    size = 32;
    spacing = 4;
    digits = digits_4x;
    break;
  }

  uint32_t digit_mask = 1;
  uint8_t digit_count = 0;
  for (size_t n = num; n; n /= 10) {
    digit_count += 1;
    digit_mask *= 10;
  }

  int8_t to_pad = padding - digit_count;
  if (to_pad < 0) {
    to_pad = 0;
  }
  if (digit_count < 1) {
    digit_count = 1;
  }

  for (uint8_t i = 0; i < to_pad; i++) {
    uint16_t x = just == JUSTIFY_LEFT
                     ? (start_x + (size + spacing) * i)
                     : (start_x - size - (size + spacing) * (digit_count - 1 - i));
    display.drawBitmap(x, y, digits[0], size, size, GxEPD_BLACK);
  }

  for (size_t i = to_pad; digit_mask > 1; digit_mask /= 10, i++) {
    uint16_t d = (num % digit_mask) * 10 / digit_mask;
    uint16_t x = just == JUSTIFY_LEFT
                     ? (start_x + (size + spacing) * i)
                     : (start_x - size - (size + spacing) * (digit_count - 1 - i));
    display.drawBitmap(x, y, digits[d], size, size, GxEPD_BLACK);
  }
}
