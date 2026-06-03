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

typedef struct
{
    I2C_HandleTypeDef *i2c;
    uint8_t address;
} OLED_Handle_t;


void OLED_init(OLED_Handle_t *oled, I2C_HandleTypeDef *hi2c);
void OLED_draw_char(OLED_Handle_t *oled, char c);
void OLED_clear(OLED_Handle_t *oled);
void OLED_set_cursor(OLED_Handle_t *oled, uint8_t page, uint8_t col);
void OLED_draw_char(OLED_Handle_t *oled, char c);
void OLED_print(OLED_Handle_t *oled, char *str);
void OLED_draw_bitmap(OLED_Handle_t *oled, const uint8_t *bitmap);
void OLED_screen_test(OLED_Handle_t *oled);



#endif /* INC_OLED_H_ */
