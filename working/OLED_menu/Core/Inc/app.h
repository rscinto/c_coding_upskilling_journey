/*
 * app.h
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "oled.h"
#include "logo.h"
#include "scd4x.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "cmsis_os2.h"


#define INDENT 10

#define CHAR_W 6

#define TEMP_VALUE_COL (9  * CHAR_W)   // "Temp (C):" = 9 chars
#define HUM_VALUE_COL  (9  * CHAR_W)   // "Hum (%): " = 9 chars
#define CO2_VALUE_COL  (11 * CHAR_W)   // "CO2 (ppm): " = 11 chars

void app_init(GPIO_TypeDef *led_port, uint16_t led_pin, I2C_HandleTypeDef *hi2c_1, I2C_HandleTypeDef *hi2c_2, I2C_HandleTypeDef *hi2c_3, UART_HandleTypeDef *serial);
void app_handle_up_button(void);
void app_handle_down_button(void);
void app_handle_left_button(void);
void app_handle_right_button(void);
void app_handle_select_button(void);
void app_update(void);
void app_update_LED(void);
void app_serial_update(void);
void app_update_sensors(void);

void app_create_rtos_objects(osMutexId_t i2c1_mutex);
void app_i2c1_lock(void);
void app_i2c1_unlock(void);

void app_update_graph_displays(void);

#endif /* INC_APP_H_ */
