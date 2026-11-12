#include "aht_driver.h"
#include "aht20.h"         // Сам внутри включает "i2c_bus.h"
#include <driver/gpio.h>
#include <stdint.h>

#define I2C_MASTER_SCL_IO   GPIO_NUM_22
#define I2C_MASTER_SDA_IO   GPIO_NUM_23
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000

static aht20_dev_handle_t aht_handle = NULL;
static i2c_bus_handle_t i2c_bus = NULL;

void aht_configure(void)
{
    const i2c_config_t i2c_bus_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &i2c_bus_conf);

    aht20_i2c_config_t i2c_conf = {
        .bus_inst = i2c_bus,
        .i2c_addr = AHT20_ADDRRES_0,
    };

    aht20_new_sensor(&i2c_conf, &aht_handle);
};


void aht20_read_data(uint32_t * temperature_raw, float * temperature, uint32_t * humidity_raw, float * humidity)
{
    aht20_read_temperature_humidity(aht_handle, temperature_raw, temperature, humidity_raw, humidity);
};