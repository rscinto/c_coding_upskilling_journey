#ifndef QMC5883L_H_
#define QMC5883L_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "sensor_common.h"
#include <math.h>



#define QMC5883L_ADDR           0x0D

#define REG_QMC_DATA_X_LSB      0x00
#define REG_QMC_DATA_X_MSB      0x01
#define REG_QMC_DATA_Y_LSB      0x02
#define REG_QMC_DATA_Y_MSB      0x03
#define REG_QMC_DATA_Z_LSB      0x04
#define REG_QMC_DATA_Z_MSB      0x05
#define REG_QMC_STATUS          0x06   // bits: X X X X X DOR OVL DRDY
#define REG_QMC_TEMP_LSB        0x07
#define REG_QMC_TEMP_MSB        0x08
#define REG_QMC_CONTROL_1       0x09  // OSR[1:0] , RNG[1:0], ODR[1:0], MODE[1:0]
#define REG_QMC_CONTROL_2       0x0A  // SOFT_RST, ROL_PNT X X X X X INT_ENB
#define REG_QMC_SET_RESET       0x0B  //write to this reg when commanding a reset. reset pin in control 2 register must be set.
#define REG_QMC_ID              0x0D  //should return 0xFF

#define QMC5883L_DEFAULT_CONTROL_1 0b11000001  //[64 OSR, 2G RNG, 10Hz ODR, continuous measurement mode]
#define QMC5883L_DEFAULT_CONTROL_2 0b00000000  // all zeroes for default power up operations

//DRDY: data register ready
//OVL: over flow detected in one of the axis
//DOR: data skip

//MODE: device operational mode (standby[00], continuous measurement[01]), standby is default on power up
//ODR: output data update rate (10Hz[00], 50Hz[01],100Hz[10] and 200Hz[11])
//RNG: magnetic field measurement range, sensitivity (2G[00], 5G[01]) (lowest field range has highest sensitivity)
//OSR: over sampling rate ( 64 [11], 128 [10], 256[01] or 512[00])

//INT_ENB: interrupt pin enabling
//ROL_PNT: point roll over function enabling
//SOFT_RST: soft reset

typedef struct
{
    uint32_t timestamp_ms;

    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;

    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;

    int16_t cal_x;
    int16_t cal_y;
    int16_t cal_z;

    float gauss_x;
    float gauss_y;
    float gauss_z;

    float heading_deg;

    bool valid;

} QMC_Measurement_t;



typedef struct
{
    I2C_HandleTypeDef *i2c;
    uint16_t address;

    QMC_Measurement_t measurement;

    Sensor_State_t state;

    uint32_t last_sample_time;
    uint32_t sample_interval_ms;

    uint32_t last_good_time;

    uint8_t failure_count;
    uint8_t max_failures;

    bool initialized;
    bool calibrated;
} QMC5883L_Handle_t;

HAL_StatusTypeDef  qmc5883l_init(QMC5883L_Handle_t *dev, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef  qmc5883l_read_measurement(QMC5883L_Handle_t *dev);
HAL_StatusTypeDef qmc5883l_calibrate(QMC5883L_Handle_t *dev, uint32_t duration_ms);
HAL_StatusTypeDef qmc5883l_update_heading(QMC5883L_Handle_t *dev);
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
