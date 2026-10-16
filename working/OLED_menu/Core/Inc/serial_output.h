/*
 * serial_output.h
 *
 *  Created on: Jun 8, 2026
 *      Author: rocco
 */

#ifndef INC_SERIAL_OUTPUT_H_
#define INC_SERIAL_OUTPUT_H_



#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    UART_HandleTypeDef *uart;

    uint32_t interval_ms;
    uint32_t last_tx_ms;

    bool header_sent;

} Serial_Output_t;

void serial_output_init(Serial_Output_t *serial,
                        UART_HandleTypeDef *uart,
                        uint32_t interval_ms);

void serial_output_update(Serial_Output_t *serial);


#endif /* INC_SERIAL_OUTPUT_H_ */
