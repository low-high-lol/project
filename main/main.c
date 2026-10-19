#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <mqtt_client.h>

#include <dht.h>
#include "float_to_buffer.h"
#include "display_spi_connection.h"


#define CONTROL_BUTTON_PIN GPIO_NUM_0
#define SENSOR_PIN GPIO_NUM_19
#define SPI_CLOCK GPIO_NUM_17
#define MOSI GPIO_NUM_16
#define MISO GPIO_NUM_18
#define DC_DISPLAY GPIO_NUM_4
#define CS_DISPLAY GPIO_NUM_2
#define RESET_DISPLAY GPIO_NUM_15

//сюда будем записывать наши данные
float temperature = 0;
float humidity = 0;


void app_main(void)
{
    
    printf("статус spi: %d \n", start_device_spi());
    
    while(1)
    {
        if(gpio_get_level(CONTROL_BUTTON_PIN) == 0)
        {
            dht_read_float_data(DHT_TYPE_AM2301, SENSOR_PIN, &humidity, &temperature); 
            printf("%f \n",humidity);
            printf("%f,\n",temperature);
            uint16_t* buffer_to_send = float_to_pixel_buffer(humidity, temperature);
            spi_display_reset();
            spi_display_paint(buffer_to_send);

        
        };

        vTaskDelay(pdMS_TO_TICKS(200));
    };


};
