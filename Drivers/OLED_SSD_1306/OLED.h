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
#include "cmsis_os.h"
#include <stdbool.h>
#include "graph_buffer.h"
#include <string.h>

#define SSD1306_ADDR 0x3C
#define FONT_WIDTH 5
#define FONT_FIRST_CHAR 32   // space
#define FONT_LAST_CHAR  122

typedef struct
{
    I2C_HandleTypeDef *i2c;
    uint8_t address;
    uint8_t framebuffer[1024];

    bool dma_busy;
    bool needs_redraw;

    const char *name;
} OLED_Handle_t;


void OLED_init(OLED_Handle_t *oled, I2C_HandleTypeDef *hi2c);
void OLED_draw_char(OLED_Handle_t *oled, char c);
void OLED_clear(OLED_Handle_t *oled);
void OLED_set_cursor(OLED_Handle_t *oled, uint8_t page, uint8_t col);
void OLED_draw_char(OLED_Handle_t *oled, char c);
void OLED_print(OLED_Handle_t *oled, char *str);
void OLED_draw_bitmap(OLED_Handle_t *oled, const uint8_t *bitmap);
void OLED_screen_test(OLED_Handle_t *oled);

void graph_buffer_init(Graph_Buffer_t *graph, float min_display, float max_display);

void OLED_clear_framebuffer(OLED_Handle_t *oled);
void OLED_draw_pixel(OLED_Handle_t *oled, uint8_t x, uint8_t y);
void OLED_draw_text_fb(OLED_Handle_t *oled, uint8_t x, uint8_t y, const char *str);
void OLED_flush_framebuffer(OLED_Handle_t *oled);
void OLED_draw_graph_fb(OLED_Handle_t *oled,
                        const Graph_Buffer_t *graph,
                        uint8_t x,
                        uint8_t y,
                        uint8_t w,
                        uint8_t h,
                        float min_value,
                        float max_value);

#endif /* INC_OLED_H_ */
