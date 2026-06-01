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

#define SCD4X_CMD_START_PERIODIC_MEASUREMENT              0x21B1
#define SCD4X_CMD_READ_MEASUREMENT                        0xEC05
#define SCD4X_CMD_STOP_PERIODIC_MEASUREMENT               0x3F86
#define SCD4X_CMD_SET_TEMPERATURE_OFFSET                  0x241D
#define SCD4X_CMD_GET_TEMPERATURE_OFFSET                  0x2318
#define SCD4X_CMD_SET_AMBIENT_PRESSURE                    0xE000
#define SCD4X_CMD_PERFORM_FORCED_RECALIBRATION            0x362F
#define SCD4X_CMD_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2416
#define SCD4X_CMD_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2313
#define SCD4X_CMD_START_LOW_POWER_PERIODIC_MEASUREMENT    0x21AC
#define SCD4X_CMD_GET_DATA_READY_STATUS                   0xE4B8
#define SCD4X_CMD_PERSIST_SETTINGS                        0x3615
#define SCD4X_CMD_GET_SERIAL_NUMBER                       0x3682
#define SCD4X_CMD_PERFORM_SELF_TEST                       0x3639
#define SCD4X_CMD_PERFORM_FACTORY_RESET                   0x3632
#define SCD4X_CMD_REINIT                                  0x3646
#define SCD4X_CMD_MEASURE_SINGLE_SHOT                     0x219D
#define SCD4X_CMD_MEASURE_SINGLE_SHOT_RHT_ONLY            0x2196
#define SCD4X_CMD_POWER_DOWN                              0x36E0
#define SCD4X_CMD_WAKE_UP                                 0x36F6

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



#endif /* INC_SCD4X_H_ */
