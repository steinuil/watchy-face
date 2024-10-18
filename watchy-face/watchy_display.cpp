#include "watchy_display.h"
#include "watchy_pins.h"

namespace watchy {
namespace impl {
static const uint32_t BUSY_TIMEOUT = 10'000'000;
static const uint32_t SPI_CLOCK_MHZ = 20'000'000;
static const uint16_t POWER_ON_TIME_MS = 100;
static const uint16_t POWER_OFF_TIME_MS = 150;
static const uint16_t FULL_REFRESH_TIME_MS = 2600;
static const uint16_t PARTIAL_REFRESH_TIME_MS = 500;

// Saves ~200ms, apparently
static constexpr bool reduceBoosterTime = true;

RTC_DATA_ATTR bool displayFullInit = true;

GxWatchy::GxWatchy()
    : GxEPD2_EPD(DISPLAY_CS, DISPLAY_DC, DISPLAY_RES, DISPLAY_BUSY, HIGH,
                 BUSY_TIMEOUT, WIDTH, HEIGHT, panel, hasColor, hasPartialUpdate,
                 hasFastPartialUpdate) {
  selectSPI(SPI, SPISettings(SPI_CLOCK_MHZ, MSBFIRST, SPI_MODE0));
  setBusyCallback(busyCallback);
}

void GxWatchy::initWatchy() { init(0, displayFullInit, 2, true); }

void GxWatchy::asyncPowerOn() {
  // This is expensive if unused
  if (!waitingPowerOn && !_hibernating) {
    _initDisplay();
    _powerOnAsync();
  }
}

void GxWatchy::busyCallback(const void *) {
  gpio_wakeup_enable((gpio_num_t)DISPLAY_BUSY, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_light_sleep_start();
}

// init controller memory and screen (default white)
void GxWatchy::clearScreen(uint8_t value) {
  writeScreenBuffer(value);
  refresh(true);
  writeScreenBufferAgain(value);
}

// init controller memory (default white)
void GxWatchy::writeScreenBuffer(uint8_t value) {
  if (!_using_partial_mode)
    _initPartial();
  if (_initial_write)
    _writeScreenBuffer(0x26, value); // set previous
  _writeScreenBuffer(0x24, value);   // set current
  _initial_write = false;            // initial full screen buffer clean done
}

// init previous buffer controller memory (default white)
void GxWatchy::writeScreenBufferAgain(uint8_t value) {
  if (!_using_partial_mode)
    _initPartial();
  _writeScreenBuffer(0x24, value); // set current
};

// write to controller memory, without screen refresh; x and w should be
// multiple of 8
void GxWatchy::writeImage(const uint8_t bitmap[], int16_t x, int16_t y,
                          int16_t w, int16_t h, bool invert, bool mirror_y,
                          bool pgm) {
  _writeImage(0x24, bitmap, x, y, w, h, invert, mirror_y, pgm);
};

void GxWatchy::writeImageForFullRefresh(const uint8_t bitmap[], int16_t x,
                                        int16_t y, int16_t w, int16_t h,
                                        bool invert, bool mirror_y, bool pgm) {
  _writeImage(0x26, bitmap, x, y, w, h, invert, mirror_y, pgm);
  _writeImage(0x24, bitmap, x, y, w, h, invert, mirror_y, pgm);
};

void GxWatchy::writeImagePart(const uint8_t bitmap[], int16_t x_part,
                              int16_t y_part, int16_t w_bitmap,
                              int16_t h_bitmap, int16_t x, int16_t y, int16_t w,
                              int16_t h, bool invert, bool mirror_y, bool pgm) {
  _writeImagePart(0x24, bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h,
                  invert, mirror_y, pgm);
};

// for differential update: set current and previous buffers equal (for fast
// partial update to work correctly)
void GxWatchy::writeImageAgain(const uint8_t bitmap[], int16_t x, int16_t y,
                               int16_t w, int16_t h, bool invert, bool mirror_y,
                               bool pgm) {
  _writeImage(0x24, bitmap, x, y, w, h, invert, mirror_y, pgm);
};

void GxWatchy::writeImagePartAgain(const uint8_t bitmap[], int16_t x_part,
                                   int16_t y_part, int16_t w_bitmap,
                                   int16_t h_bitmap, int16_t x, int16_t y,
                                   int16_t w, int16_t h, bool invert,
                                   bool mirror_y, bool pgm) {

  _writeImagePart(0x24, bitmap, x_part, y_part, w_bitmap, h_bitmap, x, y, w, h,
                  invert, mirror_y, pgm);
};

// screen refresh from controller memory to full screen
void GxWatchy::refresh(bool partialUpdateMode) {

  if (partialUpdateMode)
    refresh(0, 0, WIDTH, HEIGHT);
  else {
    if (_using_partial_mode)
      _initFull();
    _updateFull();
    _initial_refresh = false; // initial full update done
  }
};

// screen refresh from controller memory, partial screen
void GxWatchy::refresh(int16_t x, int16_t y, int16_t w, int16_t h) {

  if (_initial_refresh)
    return refresh(false); // initial update needs be full update
  // intersection with screen
  int16_t w1 = x < 0 ? w + x : w;                             // reduce
  int16_t h1 = y < 0 ? h + y : h;                             // reduce
  int16_t x1 = x < 0 ? 0 : x;                                 // limit
  int16_t y1 = y < 0 ? 0 : y;                                 // limit
  w1 = x1 + w1 < int16_t(WIDTH) ? w1 : int16_t(WIDTH) - x1;   // limit
  h1 = y1 + h1 < int16_t(HEIGHT) ? h1 : int16_t(HEIGHT) - y1; // limit
  if ((w1 <= 0) || (h1 <= 0))
    return;
  // make x1, w1 multiple of 8
  w1 += x1 % 8;
  if (w1 % 8 > 0)
    w1 += 8 - w1 % 8;
  x1 -= x1 % 8;
  if (!_using_partial_mode)
    _initPartial();
  _setPartialRamArea(x1, y1, w1, h1);
  _updatePartial();
};

// turns off generation of panel driving voltages, avoids
// screen fading over time
void GxWatchy::powerOff() { _powerOff(); };

// turns powerOff() and sets controller to deep sleep for
// minimum power use, ONLY if wakeable by RST (rst >= 0)
void GxWatchy::hibernate() { _hibernate(); };

// override
void GxWatchy::_reset() {
  // Call default method if not configured the same way
  if (_rst < 0 || !_pulldown_rst_mode) {
    GxEPD2_EPD::_reset();
    return;
  }

  digitalWrite(_rst, LOW);
  pinMode(_rst, OUTPUT);
  delay(_reset_duration);
  pinMode(_rst, INPUT_PULLUP);
  // Tested calling _powerOn() inmediately, and works ok, no need to sleep
  // delay(_reset_duration > 10 ? _reset_duration : 0);
  _hibernating = false;
}

void GxWatchy::_setPartialRamArea(uint16_t x, uint16_t y, uint16_t w,
                                  uint16_t h) {
  _startTransfer();
  _transferCommand(0x11); // set ram entry mode
  _transfer(0x03);        // x increase, y increase : normal mode
  _transferCommand(0x44);
  _transfer(x / 8);
  _transfer((x + w - 1) / 8);
  _transferCommand(0x45);
  _transfer(y % 256);
  _transfer(y / 256);
  _transfer((y + h - 1) % 256);
  _transfer((y + h - 1) / 256);
  _transferCommand(0x4e);
  _transfer(x / 8);
  _transferCommand(0x4f);
  _transfer(y % 256);
  _transfer(y / 256);
  _endTransfer();
}

void GxWatchy::_transferCommand(uint8_t value) {
  if (_dc >= 0) {
    digitalWrite(_dc, LOW);
  }

  SPI.transfer(value);

  if (_dc >= 0) {
    digitalWrite(_dc, HIGH);
  }
}

void GxWatchy::_initDisplay() {
  if (_hibernating)
    _reset();

  // No need to soft reset, the Display goes to same state after hard reset
  // _writeCommand(0x12); // soft reset
  // _waitWhileBusy("_SoftReset", 10); // 10ms max according to specs*/

  _startTransfer();
  _transferCommand(0x01); // Driver output control
  _transfer(0xC7);
  _transfer(0x00);
  _transfer(0x00);

  if (reduceBoosterTime) {
    // SSD1675B controller datasheet
    _transferCommand(0x0C); // BOOSTER_SOFT_START_CONTROL
    // Set the driving strength of GDR for all phases to maximun 0b111 -> 0xF
    // Set the minimum off time of GDR to minimum 0x4 (values below sould be
    // same)
    _transfer(0xF4); // Phase1 Default value 0x8B
    _transfer(0xF4); // Phase2 Default value 0x9C
    _transfer(0xF4); // Phase3 Default value 0x96
    _transfer(0x00); // Duration of phases, Default 0xF = 0b00 11 11 (40ms
                     // Phase 1/2, 10ms Phase 3)
  }

  _transferCommand(0x18); // Read built-in temperature sensor
  _transfer(0x80);
  _endTransfer();

  _setPartialRamArea(0, 0, WIDTH, HEIGHT);
}

void GxWatchy::_powerOnAsync() {
  if (_power_is_on)
    return;
  _startTransfer();
  _transferCommand(0x22);
  _transfer(0xf8);
  _transferCommand(0x20);
  _endTransfer();
  waitingPowerOn = true;
  _power_is_on = true;
}

void GxWatchy::_powerOn() {
  if (waitingPowerOn) {
    waitingPowerOn = false;
    _waitWhileBusy("_powerOn", POWER_ON_TIME_MS);
  }
  if (_power_is_on)
    return;
  _startTransfer();
  _transferCommand(0x22);
  _transfer(0xf8);
  _transferCommand(0x20);
  _endTransfer();
  _waitWhileBusy("_powerOn", POWER_ON_TIME_MS);
  _power_is_on = true;
}

void GxWatchy::_powerOff() {
  if (waitingPowerOn) {
    waitingPowerOn = false;
    _waitWhileBusy("_powerOn", POWER_ON_TIME_MS);
  }
  if (!_power_is_on)
    return;
  _startTransfer();
  _transferCommand(0x22);
  _transfer(0x83);
  _transferCommand(0x20);
  _endTransfer();
  _waitWhileBusy("_powerOff", POWER_OFF_TIME_MS);
  _power_is_on = false;
  _using_partial_mode = false;
}

void GxWatchy::_hibernate() {
  //_PowerOff(); // Not needed before entering deep sleep
  if (_rst >= 0) {
    _writeCommand(0x10); // deep sleep mode
    _writeData(0x1);     // enter deep sleep
    _hibernating = true;
  }
}

void GxWatchy::_initFull() {
  _initDisplay();
  _powerOn();
  _using_partial_mode = false;
}

void GxWatchy::_initPartial() {
  _initDisplay();
  _powerOn();
  _using_partial_mode = true;
}

void GxWatchy::_updatePartial() {
  _startTransfer();
  _transferCommand(0x22);
  //_transfer(0xcc); // skip temperature load (-5ms)
  _transfer(0xfc);
  _transferCommand(0x20);
  _endTransfer();
  _waitWhileBusy("_updatePartial", PARTIAL_REFRESH_TIME_MS);
}

void GxWatchy::_updateFull() {
  _startTransfer();
  _transferCommand(0x22);
  _transfer(0xf4);
  _transferCommand(0x20);
  _endTransfer();
  _waitWhileBusy("_updateFull", FULL_REFRESH_TIME_MS);
  displayFullInit = false;
}

void GxWatchy::_writeImagePart(uint8_t command, const uint8_t bitmap[],
                               int16_t x_part, int16_t y_part, int16_t w_bitmap,
                               int16_t h_bitmap, int16_t x, int16_t y,
                               int16_t w, int16_t h, bool invert, bool mirror_y,
                               bool pgm) {
  if (_initial_write)
    writeScreenBuffer(); // initial full screen buffer clean
#if defined(ESP8266) || defined(ESP32)
  yield(); // avoid wdt
#endif
  if ((w_bitmap < 0) || (h_bitmap < 0) || (w < 0) || (h < 0))
    return;
  if ((x_part < 0) || (x_part >= w_bitmap))
    return;
  if ((y_part < 0) || (y_part >= h_bitmap))
    return;
  int16_t wb_bitmap = (w_bitmap + 7) / 8; // width bytes, bitmaps are padded
  x_part -= x_part % 8;                   // byte boundary
  w = w_bitmap - x_part < w ? w_bitmap - x_part : w; // limit
  h = h_bitmap - y_part < h ? h_bitmap - y_part : h; // limit
  x -= x % 8;                                        // byte boundary
  w = 8 * ((w + 7) / 8);      // byte boundary, bitmaps are padded
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x;   // limit
  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0))
    return;
  if (!_using_partial_mode)
    _initPartial();
  _setPartialRamArea(x1, y1, w1, h1);
  _startTransfer();
  _transferCommand(command);
  for (int16_t i = 0; i < h1; i++) {
    for (int16_t j = 0; j < w1 / 8; j++) {
      uint8_t data;
      // use wb_bitmap, h_bitmap of bitmap for index!
      int16_t idx =
          mirror_y ? x_part / 8 + j + dx / 8 +
                         ((h_bitmap - 1 - (y_part + i + dy))) * wb_bitmap
                   : x_part / 8 + j + dx / 8 + (y_part + i + dy) * wb_bitmap;
      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        data = pgm_read_byte(&bitmap[idx]);
#else
        data = bitmap[idx];
#endif
      } else {
        data = bitmap[idx];
      }
      if (invert)
        data = ~data;
      _transfer(data);
    }
  }
  _endTransfer();
#if defined(ESP8266) || defined(ESP32)
  yield(); // avoid wdt
#endif
}

void GxWatchy::_writeImage(uint8_t command, const uint8_t bitmap[], int16_t x,
                           int16_t y, int16_t w, int16_t h, bool invert,
                           bool mirror_y, bool pgm) {
  if (_initial_write)
    writeScreenBuffer(); // initial full screen buffer clean
#if defined(ESP8266) || defined(ESP32)
  yield(); // avoid wdt
#endif
  int16_t wb = (w + 7) / 8;   // width bytes, bitmaps are padded
  x -= x % 8;                 // byte boundary
  w = wb * 8;                 // byte boundary
  int16_t x1 = x < 0 ? 0 : x; // limit
  int16_t y1 = y < 0 ? 0 : y; // limit
  int16_t w1 = x + w < int16_t(WIDTH) ? w : int16_t(WIDTH) - x;   // limit
  int16_t h1 = y + h < int16_t(HEIGHT) ? h : int16_t(HEIGHT) - y; // limit
  int16_t dx = x1 - x;
  int16_t dy = y1 - y;
  w1 -= dx;
  h1 -= dy;
  if ((w1 <= 0) || (h1 <= 0))
    return;
  if (!_using_partial_mode)
    _initPartial();
  _setPartialRamArea(x1, y1, w1, h1);
  _startTransfer();
  _transferCommand(command);
  for (int16_t i = 0; i < h1; i++) {
    for (int16_t j = 0; j < w1 / 8; j++) {
      uint8_t data;
      // use wb, h of bitmap for index!
      int16_t idx = mirror_y ? j + dx / 8 + ((h - 1 - (i + dy))) * wb
                             : j + dx / 8 + (i + dy) * wb;
      if (pgm) {
#if defined(__AVR) || defined(ESP8266) || defined(ESP32)
        data = pgm_read_byte(&bitmap[idx]);
#else
        data = bitmap[idx];
#endif
      } else {
        data = bitmap[idx];
      }
      if (invert)
        data = ~data;
      _transfer(data);
    }
  }
  _endTransfer();
#if defined(ESP8266) || defined(ESP32)
  yield(); // avoid wdt
#endif
}

void GxWatchy::_writeScreenBuffer(uint8_t command, uint8_t value) {
  _startTransfer();
  _transferCommand(command);
  for (uint32_t i = 0; i < uint32_t(WIDTH) * uint32_t(HEIGHT) / 8; i++) {
    _transfer(value);
  }
  _endTransfer();
}
} // namespace impl
} // namespace watchy
