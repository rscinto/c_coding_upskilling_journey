/*
 * app.h
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "oled.h"
#include <stdint.h>
#include <stdio.h>

#define INDENT 10

void menu_init();
void menu_draw();
void menu_select();
void menu_move_down();
void update_counter();
void app_init(void);
void app_handle_move_button(void);
void app_handle_select_button(void);

#endif /* INC_APP_H_ */
