#include "stdint.h"

// prepares spi bus for aht to work
void aht_configure(void);

void aht20_read_data(uint32_t *temperature_raw, float *temperature, uint32_t *humidity_raw, float *humidity);