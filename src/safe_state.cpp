// Enable non-volatile storage space
// Data persists even when power is turned off
#include "EEPROM.h"
#include "safe_state.hpp"

#define EEPROM_ADDR_LOCKED   0
#define EEPROM_ADDR_CODE_LEN 1
#define EEPROM_ADDR_CODE     2
#define EEPROM_EMPTY         0xff

#define SAFE_STATE_OPEN (char)0
#define SAFE_STATE_LOCKED (char)1

Safe_state::Safe_state() {
    this->locked_ = EEPROM.read(EEPROM_ADDR_LOCKED) == SAFE_STATE_LOCKED;
}

void Safe_state::lock() {
    this->set_lock(true);
}

bool Safe_state::locked() {
    return this->locked_;
}

bool Safe_state::has_code() {
    auto code_length = EEPROM.read(EEPROM_ADDR_CODE_LEN);
    return code_length != EEPROM_EMPTY;
}

void Safe_state::set_code(String new_code) {
    EEPROM.write(EEPROM_ADDR_CODE_LEN, new_code.length());
    for (byte i = 0; i < new_code.length(); ++i) {
        EEPROM.write(EEPROM_ADDR_CODE + i, new_code[i]);
    }
}

bool Safe_state::unlock(String code) {
    auto code_length = EEPROM.read(EEPROM_ADDR_CODE_LEN);
    if (code_length == EEPROM_EMPTY) {
        this->set_lock(false);
        return true;
    }

    if (code.length() != code_length) {
        return false;
    }

    for (byte i = 0; i < code.length(); ++i) {
        auto digit = EEPROM.read(EEPROM_ADDR_CODE + i);
        
        if (digit != code[i]) {
            return false;
        }
    }

    this->set_lock(false);
    return true;
}

void Safe_state::set_lock(bool locked) {
    this->locked_ = locked;
    EEPROM.write(EEPROM_ADDR_LOCKED, locked ? SAFE_STATE_LOCKED : SAFE_STATE_OPEN);
}