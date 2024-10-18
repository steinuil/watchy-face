#include "watchy_display.h"
#include "watchy_pins.h"
#include <Wire.h>
#include <esp_sleep.h>

namespace watchy {

enum WakeupCause : uint8_t {
  Reset = 0b0,
  ExternalRtcAlarm = 0b1,
  ButtonBottomLeft = 0b0010,
  ButtonBottomRight = 0b0110,
  ButtonTopLeft = 0b1010,
  ButtonTopRight = 0b1110,
};

static const uint8_t IS_BUTTON_MASK = 0b0010;

class Watchy {
  void initialize();

private:
  Display display;
};

void Watchy::initialize() {
  Wire.begin(SDA, SCL);
  // TODO initialize RTC?
  display.initialize();
}

WakeupCause getWakeupCause() {
  auto wakeupCause = esp_sleep_get_wakeup_cause();

  switch (wakeupCause) {
  case ESP_SLEEP_WAKEUP_EXT0:
    return WakeupCause::ExternalRtcAlarm;

  case ESP_SLEEP_WAKEUP_EXT1: {
    auto wakeupBit = esp_sleep_get_ext1_wakeup_status();

    if (wakeupBit & BUTTON_BOTTOM_LEFT_MASK) {
      return WakeupCause::ButtonBottomLeft;
    }

    if (wakeupBit & BUTTON_TOP_LEFT_MASK) {
      return WakeupCause::ButtonTopLeft;
    }

    if (wakeupBit & BUTTON_TOP_RIGHT_MASK) {
      return WakeupCause::ButtonTopRight;
    }

    if (wakeupBit & BUTTON_BOTTOM_RIGHT_MASK) {
      return WakeupCause::ButtonBottomRight;
    }
  }

  case ESP_SLEEP_WAKEUP_UNDEFINED:
  default:
    return WakeupCause::Reset;
  }
}

} // namespace watchy
