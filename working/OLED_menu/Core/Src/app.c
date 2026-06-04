/*
 * app.c
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */
#include "app.h"
#include "scd4x.h"
#include "bmp_bme280.h"
#include "hmc5883L.h"
#include "sensor_common.h"




typedef enum  //tracks which screen we're on
{
	SCREEN_MAIN_MENU, SCREEN_BLINK, SCREEN_COUNTER, SCREEN_DATA, SCREEN_BMX280, SCREEN_ABOUT
} screen_t;

typedef enum //tracks where the cursor is pointing.
{
	MENU_ITEM_BLINK,  // ==0
	MENU_ITEM_COUNTER,  // ==1
	MENU_ITEM_DATA,
	MENU_ITEM_BMX280,
	MENU_ITEM_ABOUT,
	MENU_ITEM_COUNT    //keeps track of the size of the menu for cursor wrapping
} menu_item_t;

typedef enum //tracks LED state
{
	LED_MODE_OFF,
	LED_MODE_ON,
	LED_MODE_BLINK_SLOW,
	LED_MODE_BLINK_FAST,
	LED_MODE_COUNT
} led_mode_t;

typedef struct //tracks the state of which menu item is to be selected when the select button is pressed
{
	I2C_HandleTypeDef *I2C_handle;
	I2C_HandleTypeDef *I2C_handle_graph_1;
	I2C_HandleTypeDef *I2C_handle_graph_2;
	menu_item_t selected_item;
	screen_t current_screen;
	led_mode_t led_mode;
	uint32_t counter;
	uint32_t last_blink_time;
	uint32_t blink_interval_ms;
	GPIO_TypeDef *led_port;
	uint16_t led_pin;
	SCD4X_Handle_t scd4x;
	BMP280_Handle_t bmp280;
	OLED_Handle_t OLED_main;
	OLED_Handle_t OLED_graph_1;
	OLED_Handle_t OLED_graph_2;
} app_t;

static app_t app = {
		.selected_item = MENU_ITEM_BLINK,
		.current_screen = SCREEN_MAIN_MENU,
		.led_mode = LED_MODE_OFF,
		.counter = 0,
		.last_blink_time = 0,
		.blink_interval_ms = 500,
};






static void format_press_BMX280(char *buffer, size_t size)
{
	int32_t pa = app.bmp280.data.pressure_pa;

	snprintf(buffer, size, "%ld.%02ld hPa",
	         (long)(pa / 100),
	         (long)(pa % 100));
}






static void draw_data_BMX280(void) {

	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data BMX280");

	//need some while data is good logic later.
	// need to be able to update in real time
	//char buffer[16];

	//fill buffer with clean temperature
	//format_temp(buffer, sizeof(buffer));

	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,"Pressure (Pa):");
	//OLED_print(&app.OLED_main,buffer);

}

static void update_BMX280_data(void)
{
		char buffer[16];

		//fill buffer with clean temperature
		format_press_BMX280(buffer, sizeof(buffer));
		OLED_set_cursor(&app.OLED_main,2, 0);
		OLED_print(&app.OLED_main,buffer);
}







static void update_BMX280_data_fail(void)
{
		OLED_set_cursor(&app.OLED_main,2, 0);
		OLED_print(&app.OLED_main,"      ");
		OLED_set_cursor(&app.OLED_main,2, 0);
		OLED_print(&app.OLED_main,"Error");

}



static void format_temp(char *buffer, size_t size)
{
    int temp_x100 = (int)(app.scd4x.data.temperature_c * 100.0f);

    snprintf(buffer, size, "%d.%02d",
             temp_x100 / 100,
             temp_x100 % 100);
}





static void format_humd(char *buffer, size_t size)
{
    int rh_x100 = (int)(app.scd4x.data.humidity_rh * 100.0f);

    snprintf(buffer, size, "%d.%02d",
    		rh_x100 / 100,
			rh_x100 % 100);
}






static void format_c02(char *buffer, size_t size)
{
    snprintf(buffer, size, "%d", app.scd4x.data.co2_ppm);
}





static void draw_data(void) {

	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data");

	//need some while data is good logic later.
	// need to be able to update in real time
	char buffer[16];

	//fill buffer with clean temperature
	format_temp(buffer, sizeof(buffer));

	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,"Temp (C):");
	OLED_print(&app.OLED_main,buffer);

	//fill buffer with clean humidity,
	format_humd(buffer, sizeof(buffer));

	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,"Hum (%): ");
	OLED_print(&app.OLED_main,buffer);

	//fill buffer with clean C02,
	format_c02(buffer, sizeof(buffer));

	OLED_set_cursor(&app.OLED_main,3, 0);
	OLED_print(&app.OLED_main,"CO2 (ppm): ");
	OLED_print(&app.OLED_main,buffer);
}





static void update_scd4x_data(void)
{
		char buffer[16];

		//fill buffer with clean temperature
		format_temp(buffer, sizeof(buffer));
		OLED_set_cursor(&app.OLED_main,1, TEMP_VALUE_COL);
		OLED_print(&app.OLED_main,"     ");
		OLED_set_cursor(&app.OLED_main,1, TEMP_VALUE_COL);
		OLED_print(&app.OLED_main,buffer);

		//fill buffer with clean humidity,
		format_humd(buffer, sizeof(buffer));
		OLED_set_cursor(&app.OLED_main,2, HUM_VALUE_COL);
		OLED_print(&app.OLED_main,"     ");
		OLED_set_cursor(&app.OLED_main,2, HUM_VALUE_COL);
		OLED_print(&app.OLED_main,buffer);

		//fill buffer with clean C02,
		format_c02(buffer, sizeof(buffer));
		OLED_set_cursor(&app.OLED_main,3, CO2_VALUE_COL);
		OLED_print(&app.OLED_main,"     ");
		OLED_set_cursor(&app.OLED_main,3, CO2_VALUE_COL);
		OLED_print(&app.OLED_main,buffer);
}







static void update_scd4x_data_fail(void)
{
		OLED_set_cursor(&app.OLED_main,1, TEMP_VALUE_COL);
		OLED_print(&app.OLED_main,"    ");
		OLED_set_cursor(&app.OLED_main,1, TEMP_VALUE_COL);
		OLED_print(&app.OLED_main,"Error");

		OLED_set_cursor(&app.OLED_main,2, HUM_VALUE_COL);
		OLED_print(&app.OLED_main,"    ");
		OLED_set_cursor(&app.OLED_main,2, HUM_VALUE_COL);
		OLED_print(&app.OLED_main,"Error");

		OLED_set_cursor(&app.OLED_main,3, CO2_VALUE_COL);
		OLED_print(&app.OLED_main,"    ");
		OLED_set_cursor(&app.OLED_main,3, CO2_VALUE_COL);
		OLED_print(&app.OLED_main,"Error");
}







static void menu_screen_draw(void) {
	//OLED_clear(); // causes flutter whenever we move down.

	//don't change the state. just draw please
	//app.current_screen = SCREEN_MAIN_MENU;
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Main Menu");

	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,app.selected_item == MENU_ITEM_BLINK ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,1, INDENT);
	OLED_print(&app.OLED_main,"Blink LED");

	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,app.selected_item == MENU_ITEM_COUNTER ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,2, INDENT);
	OLED_print(&app.OLED_main,"Counter");

	OLED_set_cursor(&app.OLED_main,3, 0);
	OLED_print(&app.OLED_main,app.selected_item == MENU_ITEM_DATA ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,3, INDENT);
	OLED_print(&app.OLED_main,"Data");

	OLED_set_cursor(&app.OLED_main,4, 0);
	OLED_print(&app.OLED_main,app.selected_item == MENU_ITEM_BMX280 ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,4, INDENT);
	OLED_print(&app.OLED_main,"Data BMX280");

	OLED_set_cursor(&app.OLED_main,5, 0);
	OLED_print(&app.OLED_main,app.selected_item == MENU_ITEM_ABOUT ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,5, INDENT);
	OLED_print(&app.OLED_main,"About");
}






static void update_counter() {
	app.counter++;
}






static void blink_screen_draw(void) {
	switch (app.led_mode) {
	case LED_MODE_OFF:
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"Off");
		break;
	case LED_MODE_ON:
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"On");
		break;
	case LED_MODE_BLINK_SLOW:
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"Slow");
		break;
	case LED_MODE_BLINK_FAST:
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"Fast");
		break;
	case LED_MODE_COUNT:
		break;
	}
}






static void return_to_main(void) {
	app.current_screen = SCREEN_MAIN_MENU;
	OLED_clear(&app.OLED_main);
	menu_screen_draw();
}






static void menu_select() {
	if (app.current_screen == SCREEN_MAIN_MENU) //means we're on the main menu and we need to enter one of the screens
			{
		switch (app.selected_item) {
		case MENU_ITEM_BLINK:
			app.current_screen = SCREEN_BLINK;
			OLED_clear(&app.OLED_main);
			OLED_set_cursor(&app.OLED_main,0, 0);
			OLED_print(&app.OLED_main,"Blink Mode");
			blink_screen_draw();
			// set some app state saying we are now in blink screen
			break;

		case MENU_ITEM_COUNTER: {
			app.current_screen = SCREEN_COUNTER;
			OLED_clear(&app.OLED_main);
			OLED_set_cursor(&app.OLED_main,0, 0);
			OLED_print(&app.OLED_main,"Counter");
			OLED_set_cursor(&app.OLED_main,1, INDENT);
			OLED_print(&app.OLED_main,"Button Presses");
			OLED_set_cursor(&app.OLED_main,2, INDENT);

			char buffer[12];
			snprintf(buffer, sizeof(buffer), "%lu",
					(unsigned long) app.counter);
			OLED_print(&app.OLED_main,buffer);
			break;
		}
		case MENU_ITEM_DATA: {
			app.current_screen = SCREEN_DATA;
			if(app.scd4x.data_valid == true)
			{
				draw_data();
			}
			else
			{
				draw_data();// needs to draw whole screen
				update_scd4x_data_fail(); //fill in error parts
			}
			break;
		}
		case MENU_ITEM_BMX280: {
			app.current_screen = SCREEN_BMX280;
			if(app.bmp280.data_valid == true)
			{
				draw_data_BMX280();
			}
			else
			{
				//draw_data_BMX280();// needs to draw whole screen
				draw_data_BMX280();
				update_BMX280_data_fail();
			}
			break;
		}
		case MENU_ITEM_ABOUT:
			app.current_screen = SCREEN_ABOUT;
			OLED_clear(&app.OLED_main);
			OLED_set_cursor(&app.OLED_main,1, 0);
			OLED_print(&app.OLED_main,"The hunger of a lion");
			OLED_set_cursor(&app.OLED_main,2, 0);
			OLED_print(&app.OLED_main,"The strength of a sun");
			OLED_set_cursor(&app.OLED_main,3, 0);
			OLED_print(&app.OLED_main,"She doesn't run the");
			OLED_set_cursor(&app.OLED_main,4, 0);
			OLED_print(&app.OLED_main,"track,");
			OLED_set_cursor(&app.OLED_main,5, 0);
			OLED_print(&app.OLED_main,"She makes the track");
			OLED_set_cursor(&app.OLED_main,6, 0);
			OLED_print(&app.OLED_main,"run.");
			break;
		case MENU_ITEM_COUNT: // fall through into default. // make compiler happy including MENU_ITEM_COUNT
		default:
			// should never happen
			return_to_main();
			break;
		}
	} else //we're in a screen and we need to go back to the main menu.
	{
		return_to_main();
	}

}






static void app_update_LED(void) {
	if (app.led_mode == LED_MODE_OFF) {
		HAL_GPIO_WritePin(app.led_port, app.led_pin, GPIO_PIN_RESET);
		return;
	} else if (app.led_mode == LED_MODE_ON) {
		HAL_GPIO_WritePin(app.led_port, app.led_pin, GPIO_PIN_SET);
		return;
	} else if (app.led_mode == LED_MODE_BLINK_SLOW) {
		app.blink_interval_ms = 500;
	} else if (app.led_mode == LED_MODE_BLINK_FAST) {
		app.blink_interval_ms = 250;
	}

	uint32_t now = HAL_GetTick();

	if ((now - app.last_blink_time) >= app.blink_interval_ms) {
		app.last_blink_time = now;
		HAL_GPIO_TogglePin(app.led_port, app.led_pin);
	}

}





void app_update_bmx280(void)
{

	BMP280_read_measurement(&app.bmp280);

    if (HAL_GetTick() - app.bmp280.last_sample_time >= app.bmp280.sample_interval_ms)
    {
    	app.bmp280.last_sample_time = HAL_GetTick();

        if (BMP280_read_measurement(&app.bmp280) == HAL_OK)
        {
            app.bmp280.failure_count = 0;

            if (app.current_screen == SCREEN_BMX280)
            {
            	update_BMX280_data();
            }
        }
        else
        {
        	app.bmp280.failure_count++;

            if (app.current_screen == SCREEN_BMX280)
            {
                //update_scd4x_data_fail();
            }

            if (app.bmp280.failure_count >= app.bmp280.max_failures)
            {
            	app.bmp280.failure_count = 0;
            	BMP280_init(&app.bmp280, app.I2C_handle);
            }
        }
    }

}





static void app_update_scd4x(void) {


	    if (HAL_GetTick() - app.scd4x.last_sample_time >= app.scd4x.sample_interval_ms)
	    {
	    	app.scd4x.last_sample_time = HAL_GetTick();

	        if (SCD4X_read_measurement(&app.scd4x) == HAL_OK)
	        {
	            app.scd4x.failure_count = 0;

	            if (app.current_screen == SCREEN_DATA)
	            {
	                update_scd4x_data();
	            }
	        }
	        else
	        {
	        	app.scd4x.failure_count++;

	            if (app.current_screen == SCREEN_DATA)
	            {
	                update_scd4x_data_fail();
	            }

	            if (app.scd4x.failure_count >= app.scd4x.max_failures)
	            {
	            	app.scd4x.failure_count = 0;
	                SCD4X_init(&app.scd4x, app.I2C_handle);
	            }
	        }
	    }
}






static void menu_move_down(void) //moves cursor down on main menu.
{
	app.selected_item++;

	if (app.selected_item >= MENU_ITEM_COUNT) {
		app.selected_item = MENU_ITEM_BLINK; //wrapping
	}
	menu_screen_draw();

}






void app_handle_move_button(void) {
	switch (app.current_screen) {
	case SCREEN_MAIN_MENU:
		menu_move_down();
		//menu_screen_draw(); menu move down already draws
		break;

	case SCREEN_COUNTER: // we're on the counter, and we now move back to main.
		return_to_main();
		break;

	case SCREEN_BLINK:
		app.led_mode++;

		if (app.led_mode >= LED_MODE_COUNT) {
			app.led_mode = LED_MODE_OFF;
		}

		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"          ");
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		blink_screen_draw();
		break;
	case SCREEN_DATA: // fall through and return to main.
	case SCREEN_BMX280: // fall through and return to main.
	case SCREEN_ABOUT:
		return_to_main();
		break;
	}
}






void app_handle_select_button(void) {
	update_counter(); //increments any time the select button is pressed.

	switch (app.current_screen) {
	case SCREEN_MAIN_MENU:
		menu_select();
		break;
	case SCREEN_BLINK:
		return_to_main();
		break;
	case SCREEN_COUNTER: { //case braces to definitively make the buffer have local scope.
		app.current_screen = SCREEN_COUNTER;
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"          ");
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "%lu", (unsigned long) app.counter);
		OLED_print(&app.OLED_main,buffer);
		break;
	}
	case SCREEN_ABOUT: //falls through to main
	case SCREEN_DATA: //fall through and return to main
	default: // if current state gets corrupt, just go back to main
		return_to_main();
		break;
	}

}






static void app_update_sensors(void) {

	app_update_scd4x();
	app_update_bmx280();

}






void app_update(void) {
	app_update_LED();
	app_update_sensors();

}






static void menu_init() {
	OLED_clear(&app.OLED_main);

	app.selected_item = MENU_ITEM_BLINK;
	app.current_screen = SCREEN_MAIN_MENU;
	app.led_mode = LED_MODE_OFF;
	app.counter = 0;
	menu_screen_draw();
}






void app_init(GPIO_TypeDef *led_port, uint16_t led_pin, I2C_HandleTypeDef *hi2c_1, I2C_HandleTypeDef *hi2c_2, I2C_HandleTypeDef *hi2c_3) {
	app.I2C_handle = hi2c_1;
	app.I2C_handle_graph_1 = hi2c_2;
	app.I2C_handle_graph_2 = hi2c_3;
	app.selected_item = MENU_ITEM_BLINK;
	app.current_screen = SCREEN_MAIN_MENU;
	app.counter = 0;
	app.led_mode = LED_MODE_OFF;
	app.led_pin = led_pin;
	app.led_port = led_port;

	OLED_init(&app.OLED_main, app.I2C_handle);

	SCD4X_init(&app.scd4x, app.I2C_handle);

	BMP280_init(&app.bmp280, app.I2C_handle);

	OLED_init(&app.OLED_graph_1, app.I2C_handle_graph_1);
	OLED_init(&app.OLED_graph_2, app.I2C_handle_graph_2);

	OLED_draw_bitmap(&app.OLED_main,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_main,6, 0);
	OLED_print(&app.OLED_main,"      Roctronix");
	OLED_set_cursor(&app.OLED_main,7, 0);
	OLED_print(&app.OLED_main,"v0.01");

	OLED_draw_bitmap(&app.OLED_graph_1,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_graph_1,6, 0);
	OLED_print(&app.OLED_graph_1,"      Roctronix");
	OLED_set_cursor(&app.OLED_graph_1,7, 0);
	OLED_print(&app.OLED_graph_1,"G1");

	OLED_draw_bitmap(&app.OLED_graph_2,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_graph_2,6, 0);
	OLED_print(&app.OLED_graph_2,"      Roctronix");
	OLED_set_cursor(&app.OLED_graph_2,7, 0);
	OLED_print(&app.OLED_graph_2,"G2");

	HAL_Delay(1500);
	OLED_clear(&app.OLED_main);


	menu_init();
	menu_screen_draw();
}
