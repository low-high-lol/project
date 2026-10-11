#include <stdio.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "dht.h"


#define CONTROL_BUTTON_PIN GPIO_NUM_0
#define SENSOR_PIN GPIO_NUM_15



float temperature = 0;
float humidity = 0;


int16_t* float_to_pixel_buffer(float humidity, float temperature)
{

    // переводим цифры в массив символов
    char humidity_in_characters[3];
    char temperature_in_characters[4];
    
    snprintf(humidity_in_characters, 8, "%.1f", humidity);
    snprintf(temperature_in_characters, 10, "%.1f", temperature);
    
    //
    static int16_t buffer = 123 ;
    // проходим каждый элемент массива. на каждый элемент в буффер добавляем нужную цифру или символ
    // типа if 

    return &buffer ;
};




void app_main(void)
{
    while(gpio_get_level(CONTROL_BUTTON_PIN) == 0)
    {
        dht_read_float_data(DHT_TYPE_AM2301, SENSOR_PIN, &humidity, &temperature);
        


    };
};
