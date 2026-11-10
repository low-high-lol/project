# practice_spi

Pet project to poke around with ESP32, hardware SPI, and an ILI9341 display from scratch (no LVGL or heavy graphics libs, just raw SPI transactions). Also hooks up a DHT22 sensor to display current temperature and humidity.

## How it works

1. Hit the ESP32 `BOOT` button (GPIO 0).
2. Code grabs temperature and humidity from the DHT22.
3. It paints the numbers into a flat 320x240 RGB565 framebuffer (hand-crafted segment drawing in `float_to_buffer.c`).
4. Blasts the whole buffer straight over SPI DMA to the display.

## Pinout / Wiring

| Function | Pin (ESP32) | Notes |
|---|---|---|
| SPI SCLK | GPIO 17 | |
| SPI MOSI | GPIO 16 | |
| SPI MISO | GPIO 18 | Not really used by display, but wired up |
| Display CS | GPIO 2 | |
| Display DC | GPIO 4 | Data / Command toggle |
| Display RESET | GPIO 15 | Hardware reset pin |
| DHT22 Sensor | GPIO 19 | Single-wire data |
| Button | GPIO 0 | Built-in BOOT button |

## Build & Flash

Built with ESP-IDF (target `esp32`).

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Known quirks & things to polish

- Statically allocates a 150 KB framebuffer in DRAM. Fits into ESP32 RAM for this simple test, but obviously not ideal if WiFi/BLE are brought in later.
- DC line is toggled via manual `gpio_set_level()` between SPI transactions instead of using a SPI pre-transfer callback or `esp_lcd`.
- Primitive glyph rendering (literally drawing lines loop-by-loop into an array).
