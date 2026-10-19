#include "hal/gpio_types.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>


#define CONTROL_BUTTON_PIN GPIO_NUM_0
#define SENSOR_PIN GPIO_NUM_19
#define SPI_CLOCK GPIO_NUM_17
#define MOSI GPIO_NUM_16
#define MISO GPIO_NUM_18
#define DC_DISPLAY GPIO_NUM_4
#define CS_DISPLAY GPIO_NUM_2
#define RESET_DISPLAY GPIO_NUM_15

// spi mode 0
// clock 6.35 mhz
// frame frequency 70

gpio_config_t gpio_output = 
{
    .pin_bit_mask = (1ULL << DC_DISPLAY) | (1ULL << RESET_DISPLAY),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

spi_device_handle_t display_handle;

esp_err_t start_device_spi (void)
{
    gpio_config(&gpio_output);
    static spi_bus_config_t bus_config =
    {
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .sclk_io_num = SPI_CLOCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320*240*2
    };

    spi_bus_initialize(SPI3_HOST, &bus_config,SPI_DMA_CH_AUTO);


    static spi_device_interface_config_t display_spi_config =
    {
        .clock_speed_hz = 20*1000*1000,
        .mode = 0,
        .spics_io_num = CS_DISPLAY,
        .queue_size = 7,
    };


    return spi_bus_add_device(SPI3_HOST, &display_spi_config, &display_handle);
}


// тут надо дописать:

void spi_display_reset(void) // функция reset
{
    static int8_t buffer_reset = 0x01; // в datasheet эта команда соответствует software reset
    
    gpio_set_level(DC_DISPLAY, 0); // ставим линию команда/дата на команду - на ноль

    static spi_transaction_t reset_transaction =
    {
        .length = 8,
        .tx_buffer = &buffer_reset
    };

    spi_device_transmit(display_handle, &reset_transaction);

    //в дата шите написано что после этого обязательно нужно подождать. так что я подожду.
    vTaskDelay(pdMS_TO_TICKS((120)));


    // ну ииили просто  
    // gpio_set_level(RESET_DISPLAY, 1);
    // gpio_set_level(RESET_DISPLAY, 0);

    // тут чатик говорит что после ресета ili делает дисплей спящим, поэтому ему нужно накидать по spi
    // команд на выход из сна и включение дисплея
    
};




//  функция paint

void spi_display_paint (uint16_t *buffer )
{
    // чтобы сказать этому контроллерy "нарисуй вот это" нужно шесть транзакций.
    // 1) D/C = 0, команда "принимай координаты начала"
    // 2) dc 1 и координаты начала по x y - 2 байта каждая (в моем случае 0.0)
    // 3) dc 0 команда принимай координаты конца
    // 4) dc 1 координаты конца по x y два байта каждая. (в моем случае 319 239)
    // 5) dc 0 команда принимай буффер
    // 6) dc 1 собственно буффер

    // чатик говорит что для этого нужно еще и выставить направление на пинах. чтобы dc_display выдавал наружу сигнал. че?


    // 1)

    gpio_set_level(DC_DISPLAY,0); //ставим в слушание команды

    spi_transaction_t transaction_1 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2a}
    };

    spi_device_transmit(display_handle, &transaction_1);


    //2)

    gpio_set_level(DC_DISPLAY,1); //ставим в слушание аргумента

    spi_transaction_t transaction_2 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 32,
        .tx_data = {0x00, 0x00, 0x01,0x3f}
    };

    spi_device_transmit(display_handle, &transaction_2);


    //3)

    gpio_set_level(DC_DISPLAY,0); //ставим в слушание команды

    spi_transaction_t transaction_3 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2b}
    };

    spi_device_transmit(display_handle, &transaction_3);


    //4)

    gpio_set_level(DC_DISPLAY,1); //ставим в слушание аргумента

    spi_transaction_t transaction_4 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 32,
        .tx_data = {0x00, 0x00, 0x00,0xef}
    };

    spi_device_transmit(display_handle, &transaction_4);

    //5)

    gpio_set_level(DC_DISPLAY,0); //ставим в слушание команды

    spi_transaction_t transaction_5 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2c}
    };

    spi_device_transmit(display_handle, &transaction_5);

    
    //6)

    gpio_set_level(DC_DISPLAY,1); //ставим в слушание аргумента


    spi_transaction_t transaction_6 = 
    {
        .length = 320*240*16,
        .tx_buffer = buffer
    };

    spi_device_transmit(display_handle, &transaction_6);

};
