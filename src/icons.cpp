#include "Arduino.h"
#include "icons.hpp"

// Pixel drawing of locked and unlocked icon
// Stored in flash memory, which is large and non-volatile, as opposed to RAM
const byte icon_locked[8] PROGMEM = {
    0b01110,
    0b10001,
    0b10001,
    0b11111,
    0b11011,
    0b11011,
    0b11111,
};

const byte icon_unlocked[8] PROGMEM = {
    0b01110,
    0b10000,
    0b10000,
    0b11111,
    0b11011,
    0b11011,
    0b11111,
};

// Copies icon data from from flash memory
// Uploads it to LCD memory
void init_icons(LiquidCrystal &lcd) {
    // Temporary RAM buffer, as LCD cannot read from PROGMEM directly
    byte icon[8];

    // Copy from flash memory
    // Upload to LCD
    memcpy_P(icon, icon_locked, sizeof(icon));
    lcd.createChar(ICON_LOCKED_CHAR, icon);
    memcpy_P(icon, icon_unlocked, sizeof(icon));
    lcd.createChar(ICON_UNLOCKED_CHAR, icon);
}