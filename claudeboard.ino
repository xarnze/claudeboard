#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

USBHIDKeyboard Keyboard;

constexpr unsigned long DEBOUNCE_MS = 30;
constexpr unsigned long AUTO_ENTER_INTERVAL_MS = 10000;

struct Button {
  uint8_t pin;
  uint8_t key;
  bool stablePressed;
  bool lastReadingPressed;
  unsigned long lastChangeMs;
  bool autoWasActiveAtPress;
};

Button buttons[] = {
  {17, KEY_ESC, false, false, 0, false},
  {21, KEY_RETURN, false, false, 0, false},
  {38, KEY_RETURN, false, false, 0, false},
};

bool autoEnterEnabled = false;
unsigned long nextAutoEnterMs = 0;

void sendKey(uint8_t key) {
  Keyboard.press(key);
  delay(10);
  Keyboard.release(key);
}

void startAutoEnter() {
  autoEnterEnabled = true;
  nextAutoEnterMs = millis() + AUTO_ENTER_INTERVAL_MS;
}

void stopAutoEnter() {
  autoEnterEnabled = false;
}

void handleButtonPressed(Button &button) {
  button.autoWasActiveAtPress = autoEnterEnabled;

  if (autoEnterEnabled) {
    stopAutoEnter();
  }

  sendKey(button.key);
}

void handleButtonReleased(Button &button) {
  if (button.pin == 21 && !button.autoWasActiveAtPress) {
    startAutoEnter();
  }
}

void setup() {
  for (Button &button : buttons) {
    pinMode(button.pin, INPUT_PULLUP);
    button.stablePressed = digitalRead(button.pin) == LOW;
    button.lastReadingPressed = button.stablePressed;
    button.lastChangeMs = millis();
  }

  Keyboard.begin();
  USB.begin();
}

void loop() {
  const unsigned long now = millis();

  for (Button &button : buttons) {
    const bool readingPressed = digitalRead(button.pin) == LOW;

    if (readingPressed != button.lastReadingPressed) {
      button.lastReadingPressed = readingPressed;
      button.lastChangeMs = now;
    }

    if ((now - button.lastChangeMs) >= DEBOUNCE_MS && readingPressed != button.stablePressed) {
      button.stablePressed = readingPressed;

      if (button.stablePressed) {
        handleButtonPressed(button);
      } else {
        handleButtonReleased(button);
      }
    }
  }

  if (autoEnterEnabled && now >= nextAutoEnterMs) {
    sendKey(KEY_RETURN);
    nextAutoEnterMs += AUTO_ENTER_INTERVAL_MS;
  }
}
