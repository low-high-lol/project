#pragma once 

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

esp_err_t start_device_spi (void);

void spi_display_reset();

void spi_display_paint (uint16_t *buffer );
