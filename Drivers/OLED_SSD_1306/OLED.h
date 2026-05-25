/*
 * oled.h
 *
 *  Created on: May 24, 2026
 *      Author: rocco
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define SSD1306_ADDR 0x3C
#define FONT_WIDTH 5
#define FONT_FIRST_CHAR 32   // space
#define FONT_LAST_CHAR  122
static I2C_HandleTypeDef *oled_i2c;

void OLED_init(I2C_HandleTypeDef *hi2c);
void OLED_draw_char(char c);
void OLED_print(char *str);
void OLED_draw_char_by_index(uint8_t index);
void OLED_set_cursor(uint8_t page, uint8_t col);
void OLED_clear();
void OLED_draw_bitmap(const uint8_t *bitmap);


#endif /* INC_OLED_H_ */
