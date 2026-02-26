#include "LiquidCrystal.h"
#include "Keypad.h"
#include "Servo.h"
#include "safe_state.hpp"
#include "icons.hpp"

#define SERVO_PIN 6
#define SERVO_LOCK_POS 20
#define SERVO_UNLOCK_POS 90
Servo lock_servo;

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte row_pins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte col_pins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), row_pins, col_pins, KEYPAD_ROWS, KEYPAD_COLS);

Safe_state safe_state;

void lock() {
  lock_servo.write(SERVO_LOCK_POS);
  safe_state.lock();
}

void unlock() {
  lock_servo.write(SERVO_UNLOCK_POS);
}

void show_start_up_message() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  delay(1000);

  lcd.setCursor(0, 2);
  String message = "Arduino Safe";
  for (byte i = 0; i < message.length(); ++i) {
    lcd.print(message[i]);
    delay(100);
  }
  
  delay(500);
}

String input_secret_code() {
  lcd.setCursor(5,1);
  lcd.print("[____]");
  lcd.setCursor(6, 1);
  String result = "";
  while (result.length() < 4) {
    char key = keypad.getKey();
    if ('0' <= key && key <= '9') {
      lcd.print("*");
      result += key;
    }
  }

  return result;
}

void show_wait_screen(int delay_millis) {
  lcd.setCursor(2, 1);
  lcd.print("[..........]");
  lcd.setCursor(3, 1);
  for (byte i = 0; i < 10; ++i) {
    delay(delay_millis);
    lcd.print("=");
  }
}

bool set_new_code() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter a new code:");
  String new_code = input_secret_code();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Confirm new code");
  String confirm_code = input_secret_code();

  if (new_code.equals(confirm_code)) {
    safe_state.set_code(new_code);
    
    return true;
  } else {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Code mismatch");
    lcd.setCursor(0, 1);
    lcd.print("Safe not locked!");
    delay(2000);

    return false;
  }
}

void show_unlock_message() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(ICON_UNLOCKED_CHAR);
  lcd.setCursor(4, 0);
  lcd.print("Unlocked!");
  lcd.setCursor(15, 0);
  lcd.write(ICON_UNLOCKED_CHAR);
  delay(1000);
}

void safe_unlocked_logic() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.write(ICON_UNLOCKED_CHAR);
  lcd.setCursor(2, 0);
  lcd.print(" # to lock");
  lcd.setCursor(15, 0);
  lcd.write(ICON_UNLOCKED_CHAR);

  bool new_code_needed = true;

  if (safe_state.has_code()) {
    lcd.setCursor(0, 1);
    lcd.print(" A = new code");
    new_code_needed = false;
  }

  auto key = keypad.getKey();
  while (key != 'A' && key != '#') {
    key = keypad.getKey();
  }

  bool ready_to_lock = true;
  if (key == 'A' || new_code_needed) {
    ready_to_lock = set_new_code();
  }

  if (ready_to_lock) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.write(ICON_UNLOCKED_CHAR);
    lcd.print(" ");
    lcd.write(ICON_RIGHT_ARROW);
    lcd.print(" ");
    lcd.write(ICON_LOCKED_CHAR);

    safe_state.lock();
    lock();
    show_wait_screen(100);
  }
}

void safe_locked_logic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(ICON_LOCKED_CHAR);
  lcd.print(" Safe Locked! ");
  lcd.write(ICON_LOCKED_CHAR);

  String user_code = input_secret_code();
  bool unlocked_successfully = safe_state.unlock(user_code);
  show_wait_screen(200);

  if (unlocked_successfully) {
    show_unlock_message();
    unlock();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied!");
    show_wait_screen(1000);
  }
}

void setup() {
  lcd.begin(16, 2);
  init_icons(lcd);

  lock_servo.attach(SERVO_PIN);

  // Make sure physical lock is in sync with EEPROM state
  Serial.begin(115200);
  if (safe_state.locked()) {
    lock();
  } else {
    unlock();
  }

  show_start_up_message();
}

void loop() {
  if (safe_state.locked()) {
    safe_locked_logic();
  } else {
    safe_unlocked_logic();
  }
}