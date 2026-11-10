#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>


#define CONTROL_BUTTON_PIN GPIO_NUM_0
#define SENSOR_PIN GPIO_NUM_19
#define SPI_CLOCK GPIO_NUM_17
#define MOSI GPIO_NUM_16
#define MISO GPIO_NUM_18
#define DC_DISPLAY GPIO_NUM_4 // data / command pin
#define CS_DISPLAY GPIO_NUM_2
#define RESET_DISPLAY GPIO_NUM_15

// ili display controller specs:
// spi mode 0
// clock 6.35 mhz
// frame frequency 70

spi_device_handle_t display_handle;


// this function prepares spi and adds the display to the spi host 3 
esp_err_t start_device_spi (void)
{

    // Let´s configure gpio that we will use directly 

    gpio_config_t gpio_output_dc = 
    {
        .pin_bit_mask = (1ULL << DC_DISPLAY),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_output_dc);
    

    gpio_config_t gpio_output_reset = 
    {
        .pin_bit_mask = (1ULL << RESET_DISPLAY),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_output_reset);


    // Now we create an spi bus

    spi_bus_config_t bus_config =
    {
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .sclk_io_num = SPI_CLOCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320*240*2
    };
    spi_bus_initialize(SPI3_HOST, &bus_config,SPI_DMA_CH_AUTO);


    // and add the display on the created bus

   spi_device_interface_config_t display_spi_config =
    {
        .clock_speed_hz = 20*1000*1000,
        .mode = 0,
        .spics_io_num = CS_DISPLAY,
        .queue_size = 7,
    };

    return spi_bus_add_device(SPI3_HOST, &display_spi_config, &display_handle);
;
};



// this function prepares the connected diplay for work
// it resets it, turns it on, sets landscape mode and 2 byte rgb pixel format and wakes it up
esp_err_t spi_display_prepare(void)
{

        // software reset doesnt seems to work here, so this code is not included in the build
   // spi_transaction_t reset_transaction =
   // {
   //     .flags = SPI_TRANS_USE_TXDATA,
   //     .length = 8,
   //     .tx_data = {0x01} // это код программ ресета
   // };

   // spi_device_transmit(display_handle, &reset_transaction);



    // resetting the display by giving 0 on the reset pin
    gpio_set_level(RESET_DISPLAY, 0);
    gpio_set_level(RESET_DISPLAY, 1);

    // this operation should be followed by a short delay according to the data sheet
    vTaskDelay(pdMS_TO_TICKS((120)));

    // According to the data sheet, after each reset its nesessary to awake the display and turn it on
    // So, we´ll send "sleep_out" command
    

    gpio_set_level(DC_DISPLAY, 0);  // 0 is command

    spi_transaction_t sleep_out_command =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x11} // sleep out command code 
    };
    spi_device_transmit(display_handle, &sleep_out_command);

    // this operation should be followed by a short delay according to the data sheet
    vTaskDelay(pdMS_TO_TICKS((120)));


    // now we set landscape orientation by sending  "orientation set" command followed by "landscape orientation" data

    gpio_set_level(DC_DISPLAY, 0); // command

    spi_transaction_t orientation_set_command =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x36} // set orientation command
    };
    spi_device_transmit(display_handle, &orientation_set_command);


    gpio_set_level(DC_DISPLAY, 1); // data

    spi_transaction_t orientation_landscape_data =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x28} // "landscape" data
    };
    spi_device_transmit(display_handle, &orientation_landscape_data);



    // now we need to set the display pixel format. i want the display to take 2 bytes for each pixel colour, uint16 as an rgb colour identificator

    gpio_set_level(DC_DISPLAY, 0); // command

    spi_transaction_t colour_format_set_command =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x3a} // Pixel Format Set.
    };
    spi_device_transmit(display_handle, &colour_format_set_command);


    gpio_set_level(DC_DISPLAY, 1); // data

    spi_transaction_t colour_format_set_data =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0b01010101} // data
        // bit 6-4 is for RGB config. 101 stands for 16 bit format
        // bit 2-0 is for SPI receiving format config. 101 stands for 16 bit format
        // bit 7 and 3 dont do anything
    };
    spi_device_transmit(display_handle, &colour_format_set_data);


    // now we turn this on


    gpio_set_level(DC_DISPLAY, 0);


    spi_transaction_t display_on_transaction =
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x29}
    };

    return spi_device_transmit(display_handle, &display_on_transaction);


    
};




//  this function takes the buffer and draws numbers in it

esp_err_t spi_display_paint (uint16_t *buffer )
{
    // to paint,  we need 6 transactions:
    // 1) command "listen to X start, X end "
    // 2) data of X start X end - 32 bytes
    // 3) command "listen to Y start, Y end "
    // 4) data of Y start Y end - 32 bytes
    // 5) command "now listen buffer"
    // 6) data with the buffer


    // 1)

    gpio_set_level(DC_DISPLAY,0); // command

    spi_transaction_t transaction_1 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2a}
    };
    spi_device_transmit(display_handle, &transaction_1);


    //2)

    gpio_set_level(DC_DISPLAY,1); //data

    spi_transaction_t transaction_2 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 32,
        .tx_data = {0x00, 0x00, 0x01,0x3f}
    };
    spi_device_transmit(display_handle, &transaction_2);


    //3)

    gpio_set_level(DC_DISPLAY,0); //command

    spi_transaction_t transaction_3 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2b}
    };
    spi_device_transmit(display_handle, &transaction_3);


    //4)

    gpio_set_level(DC_DISPLAY,1); //data

    spi_transaction_t transaction_4 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 32,
        .tx_data = {0x00, 0x00, 0x00,0xef}
    };
    spi_device_transmit(display_handle, &transaction_4);

    //5)

    gpio_set_level(DC_DISPLAY,0); //command

    spi_transaction_t transaction_5 = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = {0x2c}
    };
    spi_device_transmit(display_handle, &transaction_5);

    
    //6)

    gpio_set_level(DC_DISPLAY,1); //data


    spi_transaction_t transaction_6 = 
    {
        .length = 320*240*16,
        .tx_buffer = buffer
    };

    return spi_device_transmit(display_handle, &transaction_6);

};
