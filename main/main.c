#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <mqtt_client.h>

#include "aht_driver.h"
#include <driver/i2c_master.h>

#include <dht.h>
#include "esp_err.h"
#include "float_to_buffer.h"
#include "display_driver.h"


#define CONTROL_BUTTON_PIN GPIO_NUM_0 //esp32 own button (boot)

// #define SENSOR_PIN GPIO_NUM_19 // dht22 pin (it operates with single pin)


// We will fill those floats and ints with our data by reading the sensor
float temperature = 0;
float humidity = 0;

uint32_t temperature_raw = 0; 
uint32_t humidity_raw = 0;


void app_main(void)
{
    
    // Launch spi bus & add the display on it
    ESP_ERROR_CHECK(start_device_spi()); 
    
    aht_configure();
 
    // Initial config of the display - resetting, setting landscape orientation and RGB pixel format
    ESP_ERROR_CHECK(spi_display_prepare());

    // main cycle
    while(1)
    {
        if(gpio_get_level(CONTROL_BUTTON_PIN) == 0)
        {
            // This dht library function returns humidity and temperature from the dht22 sensor
            // ESP_ERROR_CHECK(dht_read_float_data(DHT_TYPE_AM2301, SENSOR_PIN, &humidity, &temperature)); 
            
            aht20_read_data(&temperature_raw, &temperature, &humidity_raw, &humidity);
            printf("H: %f \n",humidity);
            printf("T: %f,\n",temperature);

            //Creating the pixel buffer - 320 * 240 pixels, each pixel is 2 byte unsigned integer that represents an rgb colour 
            uint16_t* buffer_to_send = float_to_pixel_buffer(humidity, temperature);

            //Sending the created buffer to the display
            spi_display_paint(buffer_to_send);
        
        };

        // delay to register only the first button press
        vTaskDelay(pdMS_TO_TICKS(200));
    };


};