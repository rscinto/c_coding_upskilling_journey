#ifndef HMC5883L_H_
#define HMC5883L_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "sensor_common.h"


#define HMC5883L_ADDR              0x1E
#define QMC5883L_ADDR              0x0D

#define REG_HMC5883L_CONFIG_A               0x00
#define REG_HMC5883L_CONFIG_B               0x01
#define REG_HMC5883L_MODE                   0x02

#define REG_HMC5883L_DATA_X_MSB             0x03
#define REG_HMC5883L_DATA_X_LSB             0x04
#define REG_HMC5883L_DATA_Z_MSB             0x05
#define REG_HMC5883L_DATA_Z_LSB             0x06
#define REG_HMC5883L_DATA_Y_MSB             0x07
#define REG_HMC5883L_DATA_Y_LSB             0x08

#define REG_HMC5883L_STATUS                 0x09

#define REG_HMC5883L_ID_A                   0x0A
#define REG_HMC5883L_ID_B                   0x0B
#define REG_HMC5883L_ID_C                   0x0C


typedef struct
{
    uint32_t timestamp_ms;

    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;

    float gauss_x;
    float gauss_y;
    float gauss_z;

    float heading_deg;

    bool valid;

} HMC_Measurement_t;



typedef struct
{
    I2C_HandleTypeDef *i2c;
    uint16_t address;

    HMC_Measurement_t data;

    Sensor_State_t state;

    uint32_t last_sample_time;
    uint32_t sample_interval_ms;

    uint32_t last_good_time;

    uint8_t failure_count;
    uint8_t max_failures;

    bool data_valid;
    bool initialized;


} HMC5883L_Handle_t;

HAL_StatusTypeDef  hmc5883l_init(HMC5883L_Handle_t *dev, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef  hmc5883l_read_measurement(HMC5883L_Handle_t *dev);

/*
measurement data
last_sample_time
sample_interval_ms
failure_count
max_failures_before_reinit
last_good_time
data_is_valid
state
*/

#endif
