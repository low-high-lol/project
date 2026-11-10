#pragma once

#include <stdint.h>
void add_buffer_symbol(uint16_t* buffer, char character, int8_t position);
// функция 2 - принимает адрес на массив и рисует в нем пиксели

uint16_t* float_to_pixel_buffer(float humidity, float temperature);
//функция 1 - принимает влажность и температуру, создает массив, вызывает функцию 2 для обновления массива
//возвращает адрес на обноввленный массив