#ifndef OLED_H
#define OLED_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

void OLED_init(I2C_HandleTypeDef *hi2c);
void OLED_draw_char(char c);
void OLED_print(char *str);

#endif
