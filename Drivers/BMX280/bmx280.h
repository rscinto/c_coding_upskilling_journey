#ifndef BMP_BME280_H_
#define BMP_BME280_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "sensor_common.h"

#define BMP_BME280_ADDR_SDO_LOW 0x76
#define BMP_BME280_ADDR_SDO_HIGH 0x77
#define BMP280_ID 0x58
#define BME280_ID 0x60

#define REG_DEVICE_ID 0xD0
#define REG_STATUS 0xF3
#define REG_CTRL_MEAS 0xF4
#define REG_CONFIG 0xF5
#define REG_CTRL_HUM 0xF2 //BME280 Only
#define REG_CALIBRATION_START 0x88
#define REG_PRESS_DATA_START 0xF7

#define DEFAULT_CONFIG_REG 0b01101000
//bit {7,6,5} stand by time between readings:  011
//bit {4,3,2}: Filter coefficient of 4 as per this mapping {000/OFF, 001/2, 010/4, 011/8, 100/16}
//bit 1: Not used
//bit 0: SPI OFF

#define DEFAULT_CTRL_MEAS_REG 0b00101111
//bit {7,6,5}: osrs_t 4 as per this mapping {000/OFF, 001/1, 010/2, 011/4, 100/8, 101/16}
//bit {4,3,2}: osrs_p 4 as per this mapping {000/OFF, 001/1, 010/2, 011/4, 100/8, 101/16}
//bit {1,0}: mode 11 for normal {00/Sleep, 01 | 10 Forced mode, 11 Normal mode}



/* mental memory model not real object, just reference.
typedef struct
{
    uint8_t humidity_bytes[2]; // live at 0xFE, 0xFD BME280 Only
    uint8_t pressure_bytes[3]; // live at 0xF7, 0xF8, 0xF9
    uint8_t temp_bytes[3];     // live at 0xFA, 0xFB, 0xFC
} BMP280_Register_Map_View;

id should be 0x58 for BMP280
id should be 0x60 for BME280
*/

typedef enum
{
	UNKNOWN,
	BMP280,
	BME280
}sensor_type_t;

typedef struct
{
    float temperature_c;
    float pressure_pa;

    uint32_t timestamp_ms;

    bool valid;

} BMX280_Measurement_t;

typedef struct
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
}BMX280_Calibration_Data_t;


typedef struct
{
	sensor_type_t sensor;

	uint16_t sensor_address;

    I2C_HandleTypeDef *i2c;

    BMX280_Measurement_t data;

    int32_t t_fine;
    BMX280_Calibration_Data_t calibration_data;

    Sensor_State_t state;

    uint32_t last_sample_time;
    uint32_t sample_interval_ms;

    uint32_t last_good_time;

    uint8_t failure_count;
    uint8_t max_failures;

    bool data_valid;
    bool initialized;

} BMP280_Handle_t;

/* TODO: implement these for ease later in architecture
measurement data
last_sample_time
sample_interval_ms
failure_count
max_failures_before_reinit
last_good_time
data_is_valid
state
*/

HAL_StatusTypeDef  BMP280_init(BMP280_Handle_t *dev, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef  BMP280_read_measurement(BMP280_Handle_t *dev);

#endif
