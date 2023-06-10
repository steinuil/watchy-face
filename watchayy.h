#pragma once

#include <Watchy.h>
#include "Passing_Shot_modified.h"

enum FONT_SCALE {
  SCALE_1X = 1,
  SCALE_2X = 2,
  SCALE_4X = 4,
};

enum JUSTIFICATION {
  JUSTIFY_LEFT,
  JUSTIFY_RIGHT,
};

class Watchayy : public Watchy {
  using Watchy::Watchy;

public:
  void drawWatchFace();

private:
  void drawNumber(int64_t num, uint8_t padding, uint16_t x, uint16_t y,
                  FONT_SCALE scale, JUSTIFICATION just);

  void drawString(const char *str, uint16_t x, uint16_t y,
                  FONT_SCALE scale);
};

#define LETTER_4X(char) passing_shot_letter_##char##_4x
#define LETTER_2X(char) passing_shot_letter_##char##_2x
#define LETTER_1X(char) passing_shot_letter_##char

const uint8_t *const digits_4x[] = {
    LETTER_4X(0), LETTER_4X(1), LETTER_4X(2),          LETTER_4X(3),
    LETTER_4X(4), LETTER_4X(5), LETTER_4X(6),          LETTER_4X(7),
    LETTER_4X(8), LETTER_4X(9), passing_shot_minus_4x,
};

const uint8_t *const digits_2x[] = {
    LETTER_2X(0), LETTER_2X(1), LETTER_2X(2),          LETTER_2X(3),
    LETTER_2X(4), LETTER_2X(5), LETTER_2X(6),          LETTER_2X(7),
    LETTER_2X(8), LETTER_2X(9), passing_shot_minus_2x,
};

const uint8_t *const digits_1x[] = {
    LETTER_1X(0), LETTER_1X(1), LETTER_1X(2),      LETTER_1X(3),
    LETTER_1X(4), LETTER_1X(5), LETTER_1X(6),      LETTER_1X(7),
    LETTER_1X(8), LETTER_1X(9), passing_shot_minus};

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

const char *const months[] = {
    0,     "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC",
};

const char *const weekdays[] = {
    0, "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT",
};
