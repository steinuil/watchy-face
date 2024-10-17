#include <GxEPD2_EPD.h>
#include "driver/gpio.h"

namespace watchy {
class GxWatchy : public GxEPD2_EPD {
public:
  static const uint16_t WIDTH = 200;
  static const uint16_t HEIGHT = 200;
  static const GxEPD2::Panel panel = GxEPD2::GDEH0154D67;
  static const bool hasColor = false;
  static const bool hasPartialUpdate = true;
  static const bool hasFastPartialUpdate = true;

  GxWatchy(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

  // init controller memory and screen (default white)
  void clearScreen(uint8_t value = 0xFF);

  // init controller memory (default white)
  void writeScreenBuffer(uint8_t value = 0xFF);

  // init previous buffer controller memory (default white)
  void writeScreenBufferAgain(uint8_t value = 0xFF);

  // write to controller memory, without screen refresh; x and w should be
  // multiple of 8
  void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w,
                  int16_t h, bool invert = false, bool mirror_y = false,
                  bool pgm = false);
  void writeImageForFullRefresh(const uint8_t bitmap[], int16_t x, int16_t y,
                                int16_t w, int16_t h, bool invert = false,
                                bool mirror_y = false, bool pgm = false);
  void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part,
                      int16_t w_bitmap, int16_t h_bitmap, int16_t x, int16_t y,
                      int16_t w, int16_t h, bool invert = false,
                      bool mirror_y = false, bool pgm = false);

  // for differential update: set current and previous buffers equal (for fast
  // partial update to work correctly)
  void writeImageAgain(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w,
                       int16_t h, bool invert = false, bool mirror_y = false,
                       bool pgm = false);
  void writeImagePartAgain(const uint8_t bitmap[], int16_t x_part,
                           int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                           int16_t x, int16_t y, int16_t w, int16_t h,
                           bool invert = false, bool mirror_y = false,
                           bool pgm = false);

  // screen refresh from controller memory to full screen
  void refresh(bool partial_update_mode = false);

  // screen refresh from controller memory, partial screen
  void refresh(int16_t x, int16_t y, int16_t w, int16_t h);

  // turns off generation of panel driving voltages, avoids
  // screen fading over time
  void powerOff();

  // turns powerOff() and sets controller to deep sleep for
  // minimum power use, ONLY if wakeable by RST (rst >= 0)
  void hibernate();
};

GxWatchy::GxWatchy(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
    : GxEPD2_EPD(cs, dc, rst, busy, HIGH, 10000000, WIDTH, HEIGHT, panel,
                 hasColor, hasPartialUpdate, hasFastPartialUpdate) {
  selectSPI(SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0))
}

class Display {
public:
  Display(int16_t cs, int16_t dc, int16_t rst, int16_t busy);

private:
  GxWatchy dpy;
};

// Display::Display(int16_t cs, int16_t dc, int16_t rst, int16_t busy)
//     : dpy(GxWatchy(cs, dc, rst, busy, HIGH, 10000000, GxWatchy.WIDTH,
//                    GxWatchy.HEIGHT, GxWatchy.panel, GxWatchy.hasColor,
//                    GxWatchy.hasPartialUpdate, GxWatchy.hasFastPartialUpdate))
//                    {
//   dpy.selectSPI(SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0))
// }

// void x() {
//   auto w = GxWatchy();
// }

} // namespace watchy
