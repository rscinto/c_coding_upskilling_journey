/*
 * app.h
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "oled.h"
#include "scd4x.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>


#define INDENT 10

#define CHAR_W 6

#define TEMP_VALUE_COL (9  * CHAR_W)   // "Temp (C):" = 9 chars
#define HUM_VALUE_COL  (9  * CHAR_W)   // "Hum (%): " = 9 chars
#define CO2_VALUE_COL  (11 * CHAR_W)   // "CO2 (ppm): " = 11 chars

void app_init(GPIO_TypeDef *led_port, uint16_t led_pin, I2C_HandleTypeDef *hi2c);
void app_handle_move_button(void);
void app_handle_select_button(void);
void app_update(void);



#endif /* INC_APP_H_ */
