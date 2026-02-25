// Only compile file once
#ifndef ICONS_HPP
#define ICONS_HPP

#include "LiquidCrystal.h"

// Create custom character numbers
#define ICON_LOCKED_CHAR (byte)0
#define ICON_UNLOCKED_CHAR (byte)1

// Standard icon on LCD1602 character set
#define ICON_RIGHT_ARROW (byte)126

void init_icons(LiquidCrystal &lcd);

#endif