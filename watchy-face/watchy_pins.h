#include <cstdint>
#include <esp_bit_defs.h>

namespace watchy {
static const int16_t DISPLAY_CS = 5;
static const int16_t DISPLAY_RES = 9;
static const int16_t DISPLAY_DC = 10;
static const int16_t DISPLAY_BUSY = 19;

static const uint64_t BUTTON_BOTTOM_LEFT_MASK = BIT64(26);
static const uint64_t BUTTON_TOP_LEFT_MASK = BIT64(25);
static const uint64_t BUTTON_TOP_RIGHT_MASK = BIT64(35);
static const uint64_t BUTTON_BOTTOM_RIGHT_MASK = BIT64(4);
}
