#pragma once

#include <GxEPD2_BW.h>
#include <GxEPD2_EPD.h>

namespace watchy {
namespace impl {
class GxWatchy : public GxEPD2_EPD {
public:
  static const uint16_t WIDTH = 200;
  static const uint16_t HEIGHT = 200;
  static const GxEPD2::Panel panel = GxEPD2::GDEH0154D67;
  static const bool hasColor = false;
  static const bool hasPartialUpdate = true;
  static const bool hasFastPartialUpdate = true;

  GxWatchy();

  void initWatchy();
  void asyncPowerOn();
  static void busyCallback(const void *);

  void clearScreen(uint8_t value = 0xFF) override;
  void writeScreenBuffer(uint8_t value = 0xFF) override;
  void writeScreenBufferAgain(uint8_t value = 0xFF) override;
  void writeImage(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w,
                  int16_t h, bool invert = false, bool mirror_y = false,
                  bool pgm = false) override;
  void writeImageForFullRefresh(const uint8_t bitmap[], int16_t x, int16_t y,
                                int16_t w, int16_t h, bool invert = false,
                                bool mirror_y = false,
                                bool pgm = false) override;
  void writeImagePart(const uint8_t bitmap[], int16_t x_part, int16_t y_part,
                      int16_t w_bitmap, int16_t h_bitmap, int16_t x, int16_t y,
                      int16_t w, int16_t h, bool invert = false,
                      bool mirror_y = false, bool pgm = false) override;
  void writeImageAgain(const uint8_t bitmap[], int16_t x, int16_t y, int16_t w,
                       int16_t h, bool invert = false, bool mirror_y = false,
                       bool pgm = false) override;
  void writeImagePartAgain(const uint8_t bitmap[], int16_t x_part,
                           int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                           int16_t x, int16_t y, int16_t w, int16_t h,
                           bool invert = false, bool mirror_y = false,
                           bool pgm = false) override;
  void refresh(bool partialUpdateMode = false) override;
  void refresh(int16_t x, int16_t y, int16_t w, int16_t h) override;
  void powerOff() override;
  void hibernate() override;

private:
  void _reset() /* override */;

  bool waitingPowerOn = false;

  void _transferCommand(uint8_t value);
  void _setPartialRamArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
  void _initDisplay();
  void _powerOnAsync();
  void _powerOn();
  void _powerOff();
  void _hibernate();
  void _initFull();
  void _initPartial();
  void _updateFull();
  void _updatePartial();
  void _writeImagePart(uint8_t command, const uint8_t bitmap[], int16_t x_part,
                       int16_t y_part, int16_t w_bitmap, int16_t h_bitmap,
                       int16_t x, int16_t y, int16_t w, int16_t h, bool invert,
                       bool mirror_y, bool pgm);
  void _writeImage(uint8_t command, const uint8_t bitmap[], int16_t x,
                   int16_t y, int16_t w, int16_t h, bool invert, bool mirror_y,
                   bool pgm);
  void _writeScreenBuffer(uint8_t command, uint8_t value);
};
} // namespace impl

class Display : public GxEPD2_BW<impl::GxWatchy, impl::GxWatchy::HEIGHT> {
public:
  Display()
      : GxEPD2_BW<impl::GxWatchy, impl::GxWatchy::HEIGHT>(impl::GxWatchy()) {}

  void initialize() { epd2.initWatchy(); }
  void asyncPowerOn() { epd2.asyncPowerOn(); }
  void disableLightSleep() { epd2.setBusyCallback(0); }
  void enableLightSleep() { epd2.setBusyCallback(epd2.busyCallback); }

private:
  impl::GxWatchy epd2;
};
} // namespace watchy
