# Claudeboard

Claudeboard is a small ESP32-S2 USB HID keyboard firmware built with PlatformIO and Arduino.

## Hardware

- Board: WEMOS LOLIN S2 Mini
- Buttons:
  - GPIO 17 sends Escape
  - GPIO 21 sends Return and toggles auto-enter on release
  - GPIO 38 sends Return

Buttons are configured with `INPUT_PULLUP`, so wire each button between its GPIO pin and ground.

## Build

```sh
platformio run
```

## Upload

```sh
platformio run --target upload
```

The configured upload and monitor port is `/dev/ttyACM0` in `platformio.ini`.

## Monitor

```sh
platformio device monitor
```
