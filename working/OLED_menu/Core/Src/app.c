/*
 * app.c
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */
#include <qmc5883L.h>
#include <string.h>
#include "app.h"
#include "scd4x.h"
#include "bmp_bme280.h"
#include "qmc5883L.h"
#include "sensor_common.h"
#include "serial_output.h"
#include "cmsis_os.h"
#include "graph_buffer.h"
#include "cmsis_os.h"

#define QMC5883L_MOUNTING_OFFSET_DEG  (-90.0f)




typedef enum  //tracks which screen we're on
{
	SCREEN_MAIN_MENU, SCREEN_BLINK, SCREEN_COUNTER, SCREEN_SCD4X_DATA, SCREEN_SCD4X_MENU, SCREEN_BMX280, SCREEN_QMC5883L, SCREEN_ABOUT
} screen_t;



typedef enum //tracks where the cursor is pointing.
{
	MENU_ITEM_BLINK,  // ==0
	MENU_ITEM_COUNTER,  // ==1
	MENU_ITEM_SCD4X,
	MENU_ITEM_BMX280,
	MENU_ITEM_QMC5883L,
	MENU_ITEM_ABOUT,
	MENU_ITEM_COUNT    //keeps track of the size of the menu for cursor wrapping
} menu_item_t;


typedef enum //tracks where the cursor in submenu
{
	SUB_MENU_ITEM_DATA_SCD,// ==0
	SUB_MENU_ITEM_GRAPH_1_temperature,
	SUB_MENU_ITEM_GRAPH_1_humidity,
	SUB_MENU_ITEM_GRAPH_1_co2,
	SUB_MENU_ITEM_GRAPH_2_temperature,
	SUB_MENU_ITEM_GRAPH_2_humidity,
	SUB_MENU_ITEM_GRAPH_2_co2,
	SUB_MENU_ITEM_SENSITIVITY_SCD,
	SUB_MENU_ITEM_REFRESH_RATE_SCD,
	SUB_MENU_ITEM_COUNT_SCD    //keeps track of the size of the menu for cursor wrapping
} sub_menu_scd_item_t;

typedef enum //tracks where the cursor in submenu
{
	SUB_MENU_ITEM_DATA_BMX,// ==0
	SUB_MENU_ITEM_GRAPH_1_pressure,
	SUB_MENU_ITEM_GRAPH_2_pressure,
	SUB_MENU_ITEM_ITEM_SENSITIVITY_BMX,
	SUB_MENU_ITEM_REFRESH_RATE_BMX,
	SUB_MENU_ITEM_COUNT_BMX    //keeps track of the size of the menu for cursor wrapping
} sub_menu_bmx_item_t;

typedef enum //tracks where the cursor in submenu
{
	SUB_MENU_ITEM_DATA_QMC,// ==0
	SUB_MENU_ITEM_GRAPH_1_heading,
	SUB_MENU_ITEM_ITEM_GRAPH_1_gauss,
	SUB_MENU_ITEM_GRAPH_2_heading,
	SUB_MENU_ITEM_GRAPH_2_gauss,
	SUB_MENU_ITEM_SENSITIVITY_QMC,
	SUB_MENU_ITEM_REFRESH_RATE_QMC,
	SUB_MENU_ITEM_COUNT_QMC    //keeps track of the size of the menu for cursor wrapping
} sub_menu_qmc_item_t;

typedef enum
{
	SENSOR_SENSITIVITY_SCD_LOW,
	SENSOR_SENSITIVITY_SCD_MEDIUM,
	SENSOR_SENSITIVITY_SCD_HIGH,
	SENSOR_SENSITIVITY_SCD_COUNT,
}sensor_sensitivity_scd;

typedef enum
{
	SENSOR_SENSITIVITY_BMX_LOW,
	SENSOR_SENSITIVITY_BMX_MEDIUM,
	SENSOR_SENSITIVITY_BMX_HIGH,
	SENSOR_SENSITIVITY_BMX_COUNT,
}sensor_sensitivity_bmx;

typedef enum
{
	SENSOR_SENSITIVITY_QMC_LOW,
	SENSOR_SENSITIVITY_QMC_MEDIUM,
	SENSOR_SENSITIVITY_QMC_HIGH,
	SENSOR_SENSITIVITY_QMC_COUNT,
}sensor_sensitivity_qmc;

typedef enum
{
	SENSOR_REFRESH_RATE_SCD_LOW,
	SENSOR_REFRESH_RATE_SCD_MEDIUM,
	SENSOR_REFRESH_RATE_SCD_HIGH,
	SENSOR_REFRESH_RATE_SCD_COUNT,
}sensor_refresh_rate_scd;

typedef enum
{
	SENSOR_REFRESH_RATE_BMX_LOW,
	SENSOR_REFRESH_RATE_BMX_MEDIUM,
	SENSOR_REFRESH_RATE_BMX_HIGH,
	SENSOR_REFRESH_RATE_BMX_COUNT,
}sensor_refresh_rate_bmx;

typedef enum
{
	SENSOR_REFRESH_RATE_QMC_LOW,
	SENSOR_REFRESH_RATE_QMC_MEDIUM,
	SENSOR_REFRESH_RATE_QMC_HIGH,
	SENSOR_REFRESH_RATE_QMC_COUNT,
}sensor_refresh_rate_qmc;

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
	UART_HandleTypeDef *uart;
	menu_item_t main_menu_selected_item;
	screen_t current_screen;
	led_mode_t led_mode;
	uint32_t counter;
	uint32_t last_blink_time;
	uint32_t blink_interval_ms;
	GPIO_TypeDef *led_port;
	uint16_t led_pin;

	SCD4X_Handle_t scd4x;
	sub_menu_scd_item_t sub_menu_scd_selected_index;
	sub_menu_scd_item_t sub_menu_scd_top_item;
	BMX280_Handle_t bmx280;
	QMC5883L_Handle_t qmc5883L;

	Serial_Output_t serial;
	OLED_Handle_t OLED_main;
	OLED_Handle_t OLED_graph_1;
	OLED_Handle_t OLED_graph_2;
	Graph_Buffer_t co2_graph;
	Graph_Buffer_t pressure_graph;
	Graph_Buffer_t temperature_graph;
	Graph_Buffer_t humidity_graph;

	osMutexId_t graph_mutex;
	osMutexId_t i2c1_mutex;

} app_t;

static app_t app = {
		.main_menu_selected_item = MENU_ITEM_BLINK,
		.current_screen = SCREEN_MAIN_MENU,
		.led_mode = LED_MODE_OFF,
		.counter = 0,
		.last_blink_time = 0,
		.blink_interval_ms = 500,
};







void app_create_rtos_objects(osMutexId_t i2c1_mutex)
{
    const osMutexAttr_t graph_mutex_attributes = {
        .name = "graph_mutex"
    };

    app.graph_mutex = osMutexNew(&graph_mutex_attributes);

    app.i2c1_mutex = i2c1_mutex;
}

 void app_i2c1_lock(void)
{
    if (app.i2c1_mutex != NULL)
    {
        osStatus_t status = osMutexAcquire(app.i2c1_mutex, 100);

        if (status != osOK)
        {
            //Error_Handler(); //trap for nested locks or locks that never got returned
        }
    }
}

 void app_i2c1_unlock(void)
{
    if (app.i2c1_mutex != NULL)
    {
        osMutexRelease(app.i2c1_mutex);
    }
}




static void serial_output_send_csv(Serial_Output_t *serial)
{
    char line[256];

    snprintf(line, sizeof(line),
             "%lu,%d,%.2f,%d,%lu,%.2f,%.2f,%d,%.1f\r\n",
             (unsigned long)HAL_GetTick(),

             app.bmx280.data.valid,
             app.bmx280.data.pressure_pa,

             app.scd4x.data.valid,
             (unsigned long)app.scd4x.data.co2_ppm,
             app.scd4x.data.temperature_c,
             app.scd4x.data.humidity_rh,

             app.qmc5883L.measurement.valid,
             app.qmc5883L.measurement.heading_deg);

    HAL_UART_Transmit(serial->uart,
                      (uint8_t *)line,
                      strlen(line),
                      100);
}




static void serial_output_send_header(Serial_Output_t *serial)
{
    const char *header =
        "time_ms,"
        "bmx_ok,pressure_pa,"
        "scd_ok,co2_ppm,scd_temp_c,scd_humidity_rh,"
        "qmc_ok,heading_deg\r\n";

    HAL_UART_Transmit(serial->uart,
                      (uint8_t *)header,
                      strlen(header),
                      100);
}


void app_serial_update(void)
{
	serial_output_update(&app.serial);
}

void serial_output_update(Serial_Output_t *serial)
{
    uint32_t now = HAL_GetTick();

    if (!serial->header_sent)
    {
        serial_output_send_header(serial);
        serial->header_sent = true;
    }

    if (now - serial->last_tx_ms < serial->interval_ms)
    {
        return;
    }

    serial->last_tx_ms = now;

    serial_output_send_csv(serial);
}







static float qmc5883l_wrap_heading(float heading_deg)
{
    while (heading_deg < 0.0f)
    {
        heading_deg += 360.0f;
    }

    while (heading_deg >= 360.0f)
    {
        heading_deg -= 360.0f;
    }

    return heading_deg;
}


static void format_heading_QMC5883L(char *buffer, size_t size)
{
    float heading = app.qmc5883L.measurement.heading_deg;

    heading += QMC5883L_MOUNTING_OFFSET_DEG;
    heading = qmc5883l_wrap_heading(heading);

    snprintf(buffer, size, "%03ld deg", (long)heading);
}





static void draw_data_QMC5883L_screen(void) {


	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data QMC5883L");
	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,"Heading");
	OLED_set_cursor(&app.OLED_main,2, 0);

	char buffer[16];
	format_heading_QMC5883L(buffer, sizeof(buffer));
	OLED_print(&app.OLED_main, buffer);


}


static void calibrating_message_QMC5883L(void) {


	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data QMC5883L");
	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,"Calibrating:");
	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,"15 sec");
	OLED_set_cursor(&app.OLED_main,3, 0);
	OLED_print(&app.OLED_main,"Move device in many");
	OLED_set_cursor(&app.OLED_main,4, 0);
	OLED_print(&app.OLED_main,"orientations");


}


static void update_QMC5883L_data(void)
{


    char buffer[16];

    OLED_set_cursor(&app.OLED_main, 2, 0);
    OLED_print(&app.OLED_main, "                ");

    OLED_set_cursor(&app.OLED_main, 2, 0);
    format_heading_QMC5883L(buffer, sizeof(buffer));
    OLED_print(&app.OLED_main, buffer);


}







static void update_QMC5883L_data_fail(void)
{



	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,"      ");
	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,"Error");



}












static void format_press_BMX280(char *buffer, size_t size)
{
	int32_t pa = app.bmx280.data.pressure_pa;

	snprintf(buffer, size, "%ld.%02ld hPa",
	         (long)(pa / 100),
	         (long)(pa % 100));
}






static void draw_data_BMX280_screen(void) {


	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data BMX280");
	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,"Pressure (Pa):");
	//OLED_print(&app.OLED_main,buffer);



}

static void update_BMX280_data(void)
{


	char buffer[16];
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

#define SCD4X_MENU_TITLE_ROW      0
#define SCD4X_MENU_FIRST_ROW      1
#define SCD4X_MENU_VISIBLE_ITEMS  6

static const char *scd4x_menu_labels[SUB_MENU_ITEM_COUNT_SCD] =
{
    "SCD4X Data",
    "Temp. Graph 1",
    "Humid. Graph 1",
    "CO2 Graph 1",
    "Temp. Graph 2",
    "Humid. Graph 2",
    "CO2 Graph 2",
    "Sensitivity",
    "Refresh Rate"
};


static void scd4x_menu_screen_draw(void)
{


	for(int visible_row = 0; visible_row < SCD4X_MENU_VISIBLE_ITEMS; visible_row++)
	{
		int item_index = app.sub_menu_scd_top_item + visible_row;

		if(item_index>= SUB_MENU_ITEM_COUNT_SCD)
		{
			break;
		}

        int oled_row = visible_row + 1;   // row 0 is title, items start at row 1


        OLED_set_cursor(&app.OLED_main, oled_row, 0);

        if (item_index == app.sub_menu_scd_selected_index)
        {
            OLED_print(&app.OLED_main, ">");
        }
        else
        {
            OLED_print(&app.OLED_main, " ");
        }

        OLED_set_cursor(&app.OLED_main, oled_row, INDENT);
		OLED_print(&app.OLED_main,scd4x_menu_labels[item_index]);



	}

}

static void scd4x_menu_screen_init() {

	OLED_clear(&app.OLED_main);
	app.sub_menu_scd_selected_index = SUB_MENU_ITEM_DATA_SCD;
	app.sub_menu_scd_top_item = SUB_MENU_ITEM_DATA_SCD;

	//clear the screen
	OLED_clear(&app.OLED_main);
	//Set cursor and Print title
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"SCD40 Menu");

	scd4x_menu_screen_draw();

}


static void draw_data_scd4x_screen(void) {

	OLED_clear(&app.OLED_main);
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Data SCD4X");

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

static void scd4x_menu_move_down(void) //moves cursor down on main menu.
{
	app.sub_menu_scd_selected_index++;

	if (app.sub_menu_scd_selected_index >= SUB_MENU_ITEM_COUNT_SCD) {
		app.sub_menu_scd_selected_index = SUB_MENU_ITEM_DATA_SCD;
		app.sub_menu_scd_top_item = SUB_MENU_ITEM_DATA_SCD;
	}
	else if (app.sub_menu_scd_selected_index >= app.sub_menu_scd_top_item + SCD4X_MENU_VISIBLE_ITEMS)
	{
		app.sub_menu_scd_top_item++;
	}


	scd4x_menu_screen_draw();


}

static void scd4x_menu_move_up(void) //moves cursor up on main menu.
{

    if (app.sub_menu_scd_selected_index == SUB_MENU_ITEM_DATA_SCD)
    {
        app.sub_menu_scd_selected_index = SUB_MENU_ITEM_COUNT_SCD - 1;

        if (SUB_MENU_ITEM_COUNT_SCD > SCD4X_MENU_VISIBLE_ITEMS) //const conditions for smaller menus later for other devices
        {
            app.sub_menu_scd_top_item =
                SUB_MENU_ITEM_COUNT_SCD - SCD4X_MENU_VISIBLE_ITEMS;
        }
        else
        {
            app.sub_menu_scd_top_item = SUB_MENU_ITEM_DATA_SCD;
        }
    }
    else
    {
        app.sub_menu_scd_selected_index--;

        if (app.sub_menu_scd_selected_index < app.sub_menu_scd_top_item)
        {
            app.sub_menu_scd_top_item--;
        }
    }

    scd4x_menu_screen_draw();

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



void scd4x_menu_select(void)
{
	switch (app.sub_menu_scd_selected_index) {
	//todo add more sub menu items and actions like sending graphs to screens
	case SUB_MENU_ITEM_DATA_SCD:
		app.current_screen = SCREEN_SCD4X_DATA;
		if (app.scd4x.data.valid == true) {
			draw_data_scd4x_screen();
			update_scd4x_data();
		} else {
			draw_data_scd4x_screen(); // needs to draw whole screen
			update_scd4x_data_fail(); //fill in error parts
		}
		break;

	}
}

static void render_scd4x_graph_display_temp(OLED_Handle_t *tgt_OLED)
{
    char line[32];

    OLED_clear_framebuffer(tgt_OLED);

    OLED_draw_text_fb(tgt_OLED, 0, 0, "SCD40 Temperature");

    if (app.scd4x.data.valid)
    {
    	snprintf(line, sizeof(line),
    	         "%.1f C",
    	         app.scd4x.data.temperature_c);
    }
    else
    {
        snprintf(line, sizeof(line), "Waiting...");
    }

    OLED_draw_text_fb(tgt_OLED, 0, 10, line);

    OLED_draw_graph_fb(tgt_OLED,
                       &app.temperature_graph,
                       0,
                       24,
                       128,
                       40,
                       15.0f,
                       35.0f);

    OLED_flush_framebuffer(tgt_OLED);
}






static void render_scd4x_graph_display_rh(OLED_Handle_t *tgt_OLED)
{
    char line[32];

    OLED_clear_framebuffer(tgt_OLED);

    OLED_draw_text_fb(tgt_OLED, 0, 0, "SCD40 Humidity");

    if (app.scd4x.data.valid)
    {
    	snprintf(line, sizeof(line),
    	         "%.1f %%RH",
    	         app.scd4x.data.humidity_rh);
    }
    else
    {
        snprintf(line, sizeof(line), "Waiting...");
    }

    OLED_draw_text_fb(tgt_OLED, 0, 10, line);

    OLED_draw_graph_fb(tgt_OLED,
                       &app.humidity_graph,
                       0,
                       24,
                       128,
                       40,
                       0.0f,
                       100.0f);

    OLED_flush_framebuffer(tgt_OLED);
}




static void main_menu_screen_draw(void) {
	//OLED_clear(); // causes flutter whenever we move down.


	//don't change the state. just draw please
	//app.current_screen = SCREEN_MAIN_MENU;
	OLED_set_cursor(&app.OLED_main,0, 0);
	OLED_print(&app.OLED_main,"Main Menu");

	OLED_set_cursor(&app.OLED_main,1, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_BLINK ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,1, INDENT);
	OLED_print(&app.OLED_main,"Blink LED");

	OLED_set_cursor(&app.OLED_main,2, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_COUNTER ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,2, INDENT);
	OLED_print(&app.OLED_main,"Counter");

	OLED_set_cursor(&app.OLED_main,3, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_SCD4X ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,3, INDENT);
	OLED_print(&app.OLED_main,"Data SCD4X");

	OLED_set_cursor(&app.OLED_main,4, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_BMX280 ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,4, INDENT);
	OLED_print(&app.OLED_main,"Data BMX280");

	OLED_set_cursor(&app.OLED_main,5, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_QMC5883L ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,5, INDENT);
	OLED_print(&app.OLED_main,"Data QMC5883L");

	OLED_set_cursor(&app.OLED_main,6, 0);
	OLED_print(&app.OLED_main,app.main_menu_selected_item == MENU_ITEM_ABOUT ? ">" : " ");
	OLED_set_cursor(&app.OLED_main,6, INDENT);
	OLED_print(&app.OLED_main,"About");


}


static void menu_init() {

	OLED_clear(&app.OLED_main);

	app.main_menu_selected_item = MENU_ITEM_BLINK;
	app.current_screen = SCREEN_MAIN_MENU;
	app.led_mode = LED_MODE_OFF;
	app.counter = 0;
	main_menu_screen_draw();
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

	main_menu_screen_draw();
}





static void menu_select() {

	if (app.current_screen == SCREEN_MAIN_MENU) //means we're on the main menu and we need to enter one of the screens
			{
		switch (app.main_menu_selected_item) {
		case MENU_ITEM_BLINK:

			app.current_screen = SCREEN_BLINK;
			OLED_clear(&app.OLED_main);
			OLED_set_cursor(&app.OLED_main,0, 0);
			OLED_print(&app.OLED_main,"Blink Mode");
			blink_screen_draw();

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
		case MENU_ITEM_SCD4X: {
			app.current_screen = SCREEN_SCD4X_MENU;
			scd4x_menu_screen_init();

			scd4x_menu_screen_draw();

			break;
			}
		case MENU_ITEM_BMX280: {
			app.current_screen = SCREEN_BMX280;
			if(app.bmx280.data.valid == true)
			{
				draw_data_BMX280_screen();
				update_BMX280_data();
			}
			else
			{
				//draw_data_BMX280_screen();// needs to draw whole screen
				draw_data_BMX280_screen();
				update_BMX280_data_fail();
			}
			break;
			}
		case MENU_ITEM_QMC5883L: {
			app.current_screen = SCREEN_QMC5883L;

			if(app.qmc5883L.calibrated == false)
			{
				calibrating_message_QMC5883L();
				qmc5883l_calibrate(&app.qmc5883L, 15000);
				OLED_clear(&app.OLED_main);
				draw_data_QMC5883L_screen();
			}
			else if(app.qmc5883L.measurement.valid == true)
			{
				draw_data_QMC5883L_screen();
				update_QMC5883L_data();

			}
			else if(app.qmc5883L.measurement.valid == false)
			{
				draw_data_QMC5883L_screen();
				update_QMC5883L_data_fail();
			}
			break;
		}
		case MENU_ITEM_ABOUT:

			app.current_screen = SCREEN_ABOUT;
			OLED_clear(&app.OLED_main);
			OLED_set_cursor(&app.OLED_main,0, 0);
			OLED_print(&app.OLED_main,"Sunsets are red,");
			OLED_set_cursor(&app.OLED_main,1, 0);
			OLED_print(&app.OLED_main,"Oceans are blue,");
			OLED_set_cursor(&app.OLED_main,2, 0);
			OLED_print(&app.OLED_main,"There is no river,");
			OLED_set_cursor(&app.OLED_main,3, 0);
			OLED_print(&app.OLED_main,"That is stronger ");
			OLED_set_cursor(&app.OLED_main,4, 0);
			OLED_print(&app.OLED_main,"  than you");
			OLED_set_cursor(&app.OLED_main,5, 0);
			OLED_print(&app.OLED_main,"");

			//The stamina of a lioness
			//The elegance of a countess
			//The strength of a fortress
			//The power of a goddess

			break;
		case MENU_ITEM_COUNT: // fall through into default. // make compiler happy including MENU_ITEM_COUNT
		default:
			// should never happen
			return_to_main();
			break;
		}
	}
	else if (app.current_screen == SCREEN_SCD4X_MENU) {
		scd4x_menu_select();
	}
	else if(app.current_screen == SCREEN_COUNTER)
	{
		//do nothing
	}
	else if(app.current_screen == SCREEN_BLINK)
	{
		//do nothing
	}
	else if(app.current_screen == SCREEN_ABOUT)
	{
		//do nothing
	}
	else //we're in a screen and we need to go back to the main menu.
	{
		return_to_main();
	}

}



static void enter_current_selection(void)
{

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
	case SCREEN_ABOUT: //do nothing
		break;
	case SCREEN_SCD4X_DATA: //do nothing we're at max depth
		break;
	case SCREEN_SCD4X_MENU:
		menu_select();
		break;
	case SCREEN_BMX280: // fall through and return to main.
	case SCREEN_QMC5883L: // fall through and return to main.
	default: // if current state gets corrupt, just go back to main
		return_to_main();
		break;
	}

}




void app_update_LED(void) {
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

	//this has a HAL_OK, HAL_ERROR return. need to use it.
	///bmx280_read_measurement(&app.bmx280);

    if (HAL_GetTick() - app.bmx280.last_sample_time >= app.bmx280.sample_interval_ms)
    {
    	app.bmx280.last_sample_time = HAL_GetTick();

        if (bmx280_read_measurement(&app.bmx280) == HAL_OK)
        {
            app.bmx280.failure_count = 0;
            app.bmx280.data.valid = true;

            if (app.current_screen == SCREEN_BMX280)
            {
            	update_BMX280_data();
            }
        }
        else
        {
        	app.bmx280.failure_count++;
        	app.bmx280.data.valid = false;

            if (app.current_screen == SCREEN_BMX280)
            {
            	update_BMX280_data_fail();
            }

            if (app.bmx280.failure_count >= app.bmx280.max_failures)
            {
            	app.bmx280.failure_count = 0;
            	bmx280_init(&app.bmx280, app.I2C_handle);
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
	            app.scd4x.data.valid = true;

	            if (app.current_screen == SCREEN_SCD4X_DATA)
	            {
	                update_scd4x_data();
	            }
	        }
	        else
	        {
	        	app.scd4x.failure_count++;
	        	app.scd4x.data.valid = false;

	            if (app.current_screen == SCREEN_SCD4X_DATA)
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
	app.main_menu_selected_item++;

	if (app.main_menu_selected_item >= MENU_ITEM_COUNT) {
		app.main_menu_selected_item = MENU_ITEM_BLINK; //wrapping
	}
	main_menu_screen_draw();

}

static void menu_move_up(void) //moves cursor up on main menu.
{

    if (app.main_menu_selected_item == MENU_ITEM_BLINK)
    {
        app.main_menu_selected_item = MENU_ITEM_COUNT - 1;
    }
    else
    {
        app.main_menu_selected_item--;
    }

	main_menu_screen_draw();

}




void app_handle_up_button(void) {

	switch (app.current_screen) {
	case SCREEN_MAIN_MENU:
		menu_move_up();
		//main_menu_screen_draw(); menu move down already draws
		break;

	case SCREEN_COUNTER: // we're on the counter, and we now move back to main.
		return_to_main();
		break;

	case SCREEN_BLINK:

	    if (app.led_mode == LED_MODE_OFF)
	    {
	        app.led_mode = LED_MODE_COUNT - 1;
	    }
	    else
	    {
	        app.led_mode--;
	    }

		OLED_set_cursor(&app.OLED_main,2, INDENT);
		OLED_print(&app.OLED_main,"          ");
		OLED_set_cursor(&app.OLED_main,2, INDENT);
		blink_screen_draw();

		break;
	case SCREEN_SCD4X_DATA: // fall through and return to main.
		//at this depth this button does nothing
		break;
	case SCREEN_SCD4X_MENU:
		scd4x_menu_move_up();
		break;
	case SCREEN_BMX280: // fall through and return to main.
	case SCREEN_QMC5883L: // fall through and return to main.
	case SCREEN_ABOUT:
		return_to_main();
		break;
	}

}


void app_handle_down_button(void) {

	switch (app.current_screen) {
	case SCREEN_MAIN_MENU:
		menu_move_down();
		//main_menu_screen_draw(); menu move down already draws
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
	case SCREEN_SCD4X_DATA: // fall through and return to main.
		break; //we do nothing on the data screen
	case SCREEN_SCD4X_MENU:
		scd4x_menu_move_down();
		break;
	case SCREEN_BMX280: // fall through and return to main.
	case SCREEN_QMC5883L: // fall through and return to main.
	case SCREEN_ABOUT:
		return_to_main();
		break;
	}

}


void app_handle_left_button(void)
{

	switch (app.current_screen) {
	case SCREEN_MAIN_MENU:
		//menu_move_down();
		//main_menu_screen_draw(); menu move down already draws
		// do nothing
		break;

	case SCREEN_COUNTER: // we're on the counter, and we now move back to main.
		return_to_main();
		break;

	case SCREEN_BLINK:
		break;
	case SCREEN_SCD4X_DATA:
		app.current_screen = SCREEN_SCD4X_MENU;
		scd4x_menu_screen_init();
		scd4x_menu_screen_draw();
		break;
	case SCREEN_SCD4X_MENU:
		return_to_main();
		break;
	case SCREEN_BMX280: // fall through and return to main.
	case SCREEN_QMC5883L: // fall through and return to main.
	case SCREEN_ABOUT:
		return_to_main();
		break;
	}

}


void app_handle_right_button(void)
{

	enter_current_selection();
}





void app_handle_select_button(void) {
	update_counter(); //increments any time the select button is pressed.
	enter_current_selection();

}







static void app_update_qmc5883L(void)
{

    if (HAL_GetTick() - app.qmc5883L.last_sample_time >= app.qmc5883L.sample_interval_ms)
    {
    	app.qmc5883L.last_sample_time = HAL_GetTick();

        if (qmc5883l_update_heading(&app.qmc5883L) == HAL_OK)
        {
            app.qmc5883L.failure_count = 0;
            ;
            if (app.current_screen == SCREEN_QMC5883L)
            {
            	update_QMC5883L_data();
            }
        }
        else
        {
        	app.qmc5883L.failure_count++;

            if (app.current_screen == SCREEN_SCD4X_DATA)
            {
                //update_scd4x_data_fail();
            }

            if (app.qmc5883L.failure_count >= app.qmc5883L.max_failures)
            {
            	app.scd4x.failure_count = 0;
                //SCD4X_init(&app.scd4x, app.I2C_handle);
            }
        }

    }

}




void app_update_sensors(void)
{
    static uint32_t last_graph_sample_ms = 0;

    app_update_scd4x();
    app_update_bmx280();
    app_update_qmc5883L();

    uint32_t now = HAL_GetTick();



    if (now - last_graph_sample_ms >= GRAPH_SAMPLE_PERIOD_MS)
    {
        if (app.graph_mutex != NULL)
        {
            osMutexAcquire(app.graph_mutex, osWaitForever);
        }
        last_graph_sample_ms = now;

        graph_buffer_push(&app.co2_graph,
                          (float)app.scd4x.data.co2_ppm,
                          app.scd4x.data.valid);

        graph_buffer_push(&app.temperature_graph,
                          (float)app.scd4x.data.temperature_c,
                          app.scd4x.data.valid);

        graph_buffer_push(&app.humidity_graph,
                          app.scd4x.data.humidity_rh,
                          app.scd4x.data.valid);

        graph_buffer_push(&app.pressure_graph,
                          app.bmx280.data.pressure_pa / 100.0f,
                          app.bmx280.data.valid);
        if (app.graph_mutex != NULL)
        {
            osMutexRelease(app.graph_mutex);
        }
    }


}






void app_update(void) {
	app_update_LED();
	app_update_sensors();
	serial_output_update(&app.serial);

}













void app_init(GPIO_TypeDef *led_port, uint16_t led_pin, I2C_HandleTypeDef *hi2c_1, I2C_HandleTypeDef *hi2c_2, I2C_HandleTypeDef *hi2c_3, UART_HandleTypeDef *uart) {
	app.I2C_handle = hi2c_1;
	app.I2C_handle_graph_1 = hi2c_2;
	app.I2C_handle_graph_2 = hi2c_3;
	app.serial.uart = uart;
	app.main_menu_selected_item = MENU_ITEM_BLINK;
	app.current_screen = SCREEN_MAIN_MENU;
	app.counter = 0;
	app.led_mode = LED_MODE_OFF;
	app.led_pin = led_pin;
	app.led_port = led_port;

	app.graph_mutex = NULL;
	app.i2c1_mutex = NULL;


	//DO NOT PUSH TO BUFFER UNLESS IT HAS INIT
	graph_buffer_init(&app.co2_graph, 400.0f, 2000.0f);
	graph_buffer_init(&app.pressure_graph, 950.0f, 1050.0f);
	graph_buffer_init(&app.temperature_graph, 15.0f, 35.0f);
	graph_buffer_init(&app.humidity_graph, 0.0f, 100.0f);


	OLED_init(&app.OLED_main, app.I2C_handle);
	SCD4X_init(&app.scd4x, app.I2C_handle);
	bmx280_init(&app.bmx280, app.I2C_handle);
	qmc5883l_init(&app.qmc5883L, app.I2C_handle);

	serial_output_init(&app.serial, app.serial.uart, 5000);
	serial_output_send_header(&app.serial);
	app.serial.header_sent = true;


	OLED_init(&app.OLED_graph_1, app.I2C_handle_graph_1);
	OLED_init(&app.OLED_graph_2, app.I2C_handle_graph_2);

	OLED_draw_bitmap(&app.OLED_main,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_main,6, 0);
	OLED_print(&app.OLED_main,"      Roctronix");
	OLED_set_cursor(&app.OLED_main,7, 0);
	OLED_print(&app.OLED_main,"v0.02");

	HAL_Delay(1000);
	OLED_clear(&app.OLED_main);

	menu_init();
	main_menu_screen_draw();

	OLED_draw_bitmap(&app.OLED_graph_2,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_graph_2,6, 0);
	OLED_print(&app.OLED_graph_2,"      Roctronix");
	OLED_set_cursor(&app.OLED_graph_2,7, 0);
	OLED_print(&app.OLED_graph_2,"G1");

	OLED_draw_bitmap(&app.OLED_graph_1,boot_logo_r_128x64);
	OLED_set_cursor(&app.OLED_graph_1,6, 0);
	OLED_print(&app.OLED_graph_1,"      Roctronix");
	OLED_set_cursor(&app.OLED_graph_1,7, 0);
	OLED_print(&app.OLED_graph_1,"G2");



}

static void render_bme280_pressure_graph_display(OLED_Handle_t *tgt_OLED)
{
    char line[32];

    OLED_clear_framebuffer(tgt_OLED);

    OLED_draw_text_fb(tgt_OLED, 0, 0, "BME280 PRESS");

    if (app.bmx280.data.valid)
    {
        snprintf(line, sizeof(line), "%.1f hPa",
                 app.bmx280.data.pressure_pa / 100.0f);
    }
    else
    {
        snprintf(line, sizeof(line), "Waiting...");
    }

    OLED_draw_text_fb(tgt_OLED, 0, 10, line);

    OLED_draw_graph_fb(tgt_OLED,
                       &app.pressure_graph,
                       0,
                       24,
                       128,
                       40,
                       950.0f,
                       1050.0f);

    OLED_flush_framebuffer(tgt_OLED);
}

static void render_scd4x_graph_display_co2(OLED_Handle_t *tgt_OLED)
{
    char line[32];

    OLED_clear_framebuffer(tgt_OLED);

    OLED_draw_text_fb(tgt_OLED, 0, 0, "SCD40 CO2");

    if (app.scd4x.data.valid)
    {
        snprintf(line, sizeof(line), "%lu ppm",
                 (unsigned long)app.scd4x.data.co2_ppm);
    }
    else
    {
        snprintf(line, sizeof(line), "Waiting...");
    }

    OLED_draw_text_fb(tgt_OLED, 0, 10, line);

    OLED_draw_graph_fb(tgt_OLED,
                       &app.co2_graph,
                       0,
                       24,
                       128,
                       40,
                       400.0f,
                       2000.0f);

    OLED_flush_framebuffer(tgt_OLED);
}


void app_update_graph_displays(void)
{
    static uint32_t last_scd_draw_ms = 0;

    uint32_t now = HAL_GetTick();

    if (now - last_scd_draw_ms >= 1000)
    {
        last_scd_draw_ms = now;
        //if (app.graph_mutex != NULL)
        //{
        //    osMutexAcquire(app.graph_mutex, osWaitForever);
        //}

			render_scd4x_graph_display_co2(&app.OLED_graph_1);
			render_scd4x_graph_display_temp(&app.OLED_graph_2);

        //if (app.graph_mutex != NULL)
        //{
        //    osMutexRelease(app.graph_mutex);
        //}
    }

}

/*
void app_update_graph_displays(void)
{
    static uint32_t last_draw_ms = 0;
    static uint32_t draw_count = 0;

    uint32_t now = HAL_GetTick();

    if (now - last_draw_ms < 1000)
    {
        return;
    }

    last_draw_ms = now;
    draw_count++;

    char line[32];

    OLED_clear(&app.OLED_graph_1);
    OLED_set_cursor(&app.OLED_graph_1, 0, 0);
    OLED_print(&app.OLED_graph_1, "GRAPH TASK RUN");
    OLED_set_cursor(&app.OLED_graph_1, 2, 0);
    snprintf(line, sizeof(line), "count %lu", (unsigned long)draw_count);
    OLED_print(&app.OLED_graph_1, line);

    OLED_clear(&app.OLED_graph_2);
    OLED_set_cursor(&app.OLED_graph_2, 0, 0);
    OLED_print(&app.OLED_graph_2, "GRAPH TASK RUN");
    OLED_set_cursor(&app.OLED_graph_2, 2, 0);
    snprintf(line, sizeof(line), "count %lu", (unsigned long)draw_count);
    OLED_print(&app.OLED_graph_2, line);
}
*/
