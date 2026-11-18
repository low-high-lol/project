# practice_spi

Pet project to poke around with ESP32, hardware SPI, I2C, Wi-Fi/MQTT, and an ILI9341 display from scratch (no LVGL or heavy graphics libs, just raw SPI transactions). Reads temperature and humidity from an **AHT20** sensor (migrated from DHT22), renders them to the screen, and publishes the telemetry over **MQTTS**.

## Project Structure

The codebase is modularized into custom ESP-IDF components:

- `main/` — Application entry point, Wi-Fi station setup, MQTTS client configuration (with TLS certificate bundle & Last Will), and the main button polling loop.
- `components/display_driver/` — Raw SPI driver (`SPI3_HOST` + DMA) for the ILI9341 controller: hardware reset, `MADCTL` orientation setup, RGB565 pixel format configuration, and partial window painting (`spi_display_paint`).
- `components/float_to_buffer/` — Minimalist software renderer (`float_to_pixel_buffer`) that draws `h` / `t` indicators and 7-segment-style digits into a static `320x120` RGB565 half-screen buffer (`76.8 KB`).
- `components/aht_driver/` — Wrapper around `espressif/aht20` and `i2c_bus` to initialize `I2C_NUM_0` and read calibrated temperature & humidity values.

## How it works

1. On boot, the ESP32 initializes NVS, connects to Wi-Fi, and starts an MQTTS client (publishing `"Sensor connected"` to `/temp`, with a Last Will of `"Sensor disconnected"`).
2. It initializes the SPI bus (`SPI3_HOST`), prepares the ILI9341 display in 180°-rotated landscape mode, turns on the backlight, and sets up the AHT20 sensor over I2C.
3. When you press the ESP32 `BOOT` button (`GPIO 0`):
   - Reads current temperature and humidity from the **AHT20** sensor.
   - Renders the humidity row (`h XX.X`) into the `320x120` pixel buffer and flushes it via SPI DMA to the top half of the screen (`Y: 0..119`).
   - Renders the temperature row (`t XX.X`) into the same buffer and flushes it to the bottom half (`Y: 120..239`).
   - Publishes `"H: XX.XX, T:XX.XX"` to the `/temp` MQTT topic (QoS 2, retain 1).

## Pinout / Wiring

| Function | Pin (ESP32) | Notes |
|---|---|---|
| **Display (SPI3)** | | |
| SPI SCLK | GPIO 21 | 4 MHz clock, SPI Mode 0 |
| SPI MOSI | GPIO 19 | |
| SPI MISO | GPIO 23 | Configured on bus |
| Display CS | GPIO 17 | |
| Display DC | GPIO 18 | Data / Command toggle |
| Display RESET | GPIO 5 | Hardware reset pin |
| Display LED | GPIO 22 | Backlight control (pulled high on init) |
| **AHT20 Sensor (I2C0)** | | |
| I2C SCL | GPIO 33 | 100 kHz, internal pull-up enabled |
| I2C SDA | GPIO 32 | Internal pull-up enabled |
| **Controls** | | |
| Button | GPIO 0 | Built-in `BOOT` button (active low) |

## Build & Flash

Built with ESP-IDF (target `esp32`). Managed dependencies (`espressif/aht20`, `espressif/mqtt`, `esp-idf-lib/dht`) are fetched automatically via the IDF Component Manager.

Before building, update `WIFI_SSID`, `WIFI_PASSWORD`, `MQTT_USER`, `MQTT_PASSWORD`, and the broker URI in `main/main.c`.

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## Known quirks & things to polish

- **RAM footprint optimization:** Instead of a full `320x240` (`153.6 KB`) framebuffer—which wouldn't fit alongside the Wi-Fi and TLS/MQTT stacks—the renderer now reuses a half-screen `320x120` (`76.8 KB`) static buffer and paints the screen in two passes.
- **Manual DC toggling:** The `DC` line is toggled via `gpio_set_level()` between synchronous SPI transactions rather than using SPI pre-transfer callbacks or `esp_lcd`.
- **Hardcoded credentials & glyph rendering:** Wi-Fi/MQTT credentials live directly in `main.c`, and digits are rasterized loop-by-loop into the 1D array.
