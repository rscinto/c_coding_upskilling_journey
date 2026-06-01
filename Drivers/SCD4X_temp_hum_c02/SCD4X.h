/*
 * scd4x.h
 *
 *  Created on: May 24, 2026
 *      Author: rocco
 */

#ifndef INC_SCD4X_H_
#define INC_SCD4X_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
//#include <stdint.h>

#define SCD4X_ADDR 0x62

//Check sum variables
#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xFF

typedef struct
{
    uint16_t co2_ppm;
    float temperature_c;
    float humidity_rh;
    bool sensor_OK;
} SCD4X_Measurement_t;



void SCD4X_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef  SCD4X_start_periodic_measurement();  //0x21b1 send command
HAL_StatusTypeDef SCD4X_read_measurement(SCD4X_Measurement_t *out);		  //0xec05 read, 1ms execution // poll every 5 seconds
HAL_StatusTypeDef  SCD4X_stop_periodic_measurement();   //0x3f86 send command, 500ms execution
uint8_t SCD4X_generate_crc(const uint8_t* data, uint16_t count);
void SCD4X_calculate_checksum();


#endif /* INC_SCD4X_H_ */
