/*
 * serial_output.c
 *
 *  Created on: Jun 8, 2026
 *      Author: rocco
 */


#include "serial_output.h"
#include "app.h"
#include <stdio.h>
#include <string.h>

void serial_output_init(Serial_Output_t *serial,
                        UART_HandleTypeDef *uart,
                        uint32_t interval_ms)
{
    serial->uart = uart;
    serial->interval_ms = interval_ms;
    serial->last_tx_ms = 0;
    serial->header_sent = false;
}

/*
static void serial_output_send_header(Serial_Output_t *serial)
{
    const char *header =
        "time_ms,bmp_ok,bmp_temp_c,bmp_pressure_hpa,"
        "scd_ok,co2_ppm,scd_temp_c,scd_rh,"
        "mag_ok,mag_x,mag_y,mag_z,heading_deg\r\n";

    HAL_UART_Transmit(serial->uart,
                      (uint8_t *)header,
                      strlen(header),
                      100);
}
*/









