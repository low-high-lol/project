#pragma once 

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

esp_err_t start_device_spi(void);

esp_err_t spi_display_prepare(void);

esp_err_t spi_display_paint(uint16_t *buffer );
