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
