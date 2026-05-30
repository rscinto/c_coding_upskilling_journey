/*
 * app.c
 *
 *  Created on: May 25, 2026
 *      Author: rocco
 */
#include "app.h"

typedef enum  //tracks which screen we're on
{
    SCREEN_MAIN_MENU,
    SCREEN_BLINK,
    SCREEN_COUNTER,
    SCREEN_ABOUT
}screen_t;

typedef enum //tracks where the cursor is pointing.
{
    MENU_ITEM_BLINK,  // ==0
    MENU_ITEM_COUNTER,// ==1
    MENU_ITEM_ABOUT,  // ==2
    MENU_ITEM_COUNT   // ==3   //keeps track of the size of the menu for cursor wrapping
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
    menu_item_t selected_item;
    screen_t current_screen;
    led_mode_t led_mode;
    uint32_t counter;

    uint32_t last_blink_time;
    uint32_t blink_interval_ms;

    GPIO_TypeDef *led_port;
    uint16_t led_pin;
} app_t;


static app_t app =
{
    .selected_item = MENU_ITEM_BLINK,
	.current_screen = SCREEN_MAIN_MENU,
	.led_mode = LED_MODE_OFF,
    .counter = 0,
    .last_blink_time = 0,
    .blink_interval_ms = 500
};

void menu_init()
{
    OLED_clear();

    app.selected_item = MENU_ITEM_BLINK;
    app.current_screen = SCREEN_MAIN_MENU;
    app.led_mode = LED_MODE_OFF;
    app.counter = 0;

    menu_screen_draw();

}

void menu_select()
{
	if(app.current_screen == SCREEN_MAIN_MENU) //means we're on the main menu and we need to enter one of the screens
	{
		switch (app.selected_item)
			    {
			        case MENU_ITEM_BLINK:
			        	app.current_screen = SCREEN_BLINK;
			        	OLED_clear();
			        	OLED_set_cursor(0,0);
			        	OLED_print("Blink Mode");
			            blink_screen_draw();
			            // set some app state saying we are now in blink screen
			            break;

			        case MENU_ITEM_COUNTER:
			        	app.current_screen = SCREEN_COUNTER;
			        	OLED_clear();
			        	OLED_set_cursor(0,0);
			        	OLED_print("Counter");
			        	OLED_set_cursor(1,INDENT);
			        	OLED_print("Button Presses");
			        	OLED_set_cursor(2,INDENT);

			        	char buffer[12];
			        	snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)app.counter);
			        	OLED_print(buffer);
			            break;

			        case MENU_ITEM_ABOUT:
			        	app.current_screen = SCREEN_ABOUT;
			        	OLED_clear();
			        	OLED_set_cursor(0,0);
			        	OLED_print("STM32 OLED UI");
			        	OLED_set_cursor(1,0);
			        	OLED_print("By Rocco");
			            break;
			        case MENU_ITEM_COUNT:// fall through into default. // make compiler happy including MENU_ITEM_COUNT
			        default:
			            // should never happen
			        	return_to_main();
			            break;
			    }
	}
	else //we're in a screen and we need to go back to the main menu.
	{
		return_to_main();
	}

}

void menu_move_down(void) //moves cursor down on main menu.
{
	    app.selected_item++;

	    if (app.selected_item >= MENU_ITEM_COUNT)
	    {
	        app.selected_item = MENU_ITEM_BLINK; //wrapping
	    }
		menu_screen_draw();

}

void menu_screen_draw(void)
{
    //OLED_clear(); // causes flutter whenever we move down.

	//don't change the state. just draw please
	//app.current_screen = SCREEN_MAIN_MENU;

    OLED_set_cursor(0, 0);
    OLED_print("Main Menu");

    OLED_set_cursor(1, 0);
    OLED_print(app.selected_item == MENU_ITEM_BLINK ? ">" : " ");
    OLED_set_cursor(1, INDENT);
    OLED_print("Blink LED");

    OLED_set_cursor(2, 0);
    OLED_print(app.selected_item == MENU_ITEM_COUNTER ? ">" : " ");
    OLED_set_cursor(2, INDENT);
    OLED_print("Counter");

    OLED_set_cursor(3, 0);
    OLED_print(app.selected_item == MENU_ITEM_ABOUT ? ">" : " ");
    OLED_set_cursor(3, INDENT);
    OLED_print("About");
}

void update_counter()
{
	app.counter++;
}


void app_init(GPIO_TypeDef *led_port, uint16_t led_pin)
{
	app.selected_item = MENU_ITEM_BLINK;
	app.current_screen = SCREEN_MAIN_MENU;
	app.counter = 0;
	app.led_mode = LED_MODE_OFF;
	app.led_pin = led_pin;
	app.led_port = led_port;

	OLED_clear();
	menu_screen_draw();
}

void app_handle_move_button(void)
{
    switch (app.current_screen)
    {
        case SCREEN_MAIN_MENU:
        	menu_move_down();
        	//menu_screen_draw(); menu move down already draws
            break;

        case SCREEN_COUNTER: // we're on the counter, and we now move back to main.
        	return_to_main();
            break;

        case SCREEN_BLINK:
            app.led_mode++;

            if (app.led_mode >= LED_MODE_COUNT)
            {
                app.led_mode = LED_MODE_OFF;
            }

            OLED_set_cursor(2, INDENT);
            OLED_print("          ");
            OLED_set_cursor(2, INDENT);
            blink_screen_draw();
        	break;

        case SCREEN_ABOUT:
        	return_to_main();
            break;
    }
}






void app_handle_select_button(void)
{
	update_counter(); //increments any time the select button is pressed.

	    switch (app.current_screen)
	    {
	        case SCREEN_MAIN_MENU:
	        	menu_select();
	            break;
	        case SCREEN_BLINK:
	        	return_to_main();
	        	break;
	        case SCREEN_COUNTER:
	        {//case braces to definitively make the buffer have local scope.
	        	app.current_screen = SCREEN_COUNTER;
	        	OLED_set_cursor(2,INDENT);
	        	OLED_print("          ");
	        	OLED_set_cursor(2,INDENT);
	        	char buffer[12];
	        	snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)app.counter);
	        	OLED_print(buffer);
	        	break;
	        }
	        case SCREEN_ABOUT://falls through to main
	        default:// if current state gets corrupt, just go back to main
	        	return_to_main();
	            break;
	    }

}

//LED_MODE_OFF,
//LED_MODE_ON,
//LED_MODE_BLINK_SLOW,
//LED_MODE_BLINK_FAST
//LED_MODE_COUNT
void app_update(void)
{
    if (app.led_mode == LED_MODE_OFF)
    {
        HAL_GPIO_WritePin(app.led_port, app.led_pin, GPIO_PIN_RESET);
        return;
    }
    else if(app.led_mode == LED_MODE_ON)
    {
    	HAL_GPIO_TogglePin(app.led_port, app.led_pin);
    	return;
    }
    else if(app.led_mode == LED_MODE_BLINK_SLOW)
    {
    	app.blink_interval_ms = 500;
    }
    else if(app.led_mode == LED_MODE_BLINK_FAST)
    {
    	app.blink_interval_ms = 250;
    }


    uint32_t now = HAL_GetTick();

    if ((now - app.last_blink_time) >= app.blink_interval_ms)
    {
        app.last_blink_time = now;
        HAL_GPIO_TogglePin(app.led_port, app.led_pin);
    }
}




void blink_screen_draw(void)
{
	switch(app.led_mode)
	{
	case LED_MODE_OFF:
		OLED_set_cursor(2,INDENT);
		OLED_print("Off");
		break;
	case LED_MODE_ON:
		OLED_set_cursor(2,INDENT);
		OLED_print("On");
		break;
	case LED_MODE_BLINK_SLOW:
		OLED_set_cursor(2,INDENT);
		OLED_print("Slow");
		break;
	case LED_MODE_BLINK_FAST:
		OLED_set_cursor(2,INDENT);
		OLED_print("Fast");
		break;
	case LED_MODE_COUNT:
		break;
	}
}


void return_to_main(void)
{
    app.current_screen = SCREEN_MAIN_MENU;
    OLED_clear();
    menu_screen_draw();
}