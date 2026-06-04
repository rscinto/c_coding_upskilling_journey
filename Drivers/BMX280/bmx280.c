#include "bmp_bme280.h"



static HAL_StatusTypeDef  BMX280_read_calibration(BMP280_Handle_t *dev)
{

	uint8_t raw_cal_byte[24];

	if(HAL_I2C_Mem_Read(dev->i2c, dev->sensor_address << 1,
		REG_CALIBRATION_START,
		I2C_MEMADD_SIZE_8BIT,
		raw_cal_byte,
		24,
		HAL_MAX_DELAY) == HAL_ERROR)
	{
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
	}

	uint16_t raw_cal_full[12];

	int j = 0;
	for(int i = 0; i<12;i++)
	{
		raw_cal_full[i] = raw_cal_byte[++j];
		raw_cal_full[i] = raw_cal_full[i] << 8;
		raw_cal_full[i] = (raw_cal_full[i] & 0xFF00) | raw_cal_byte[--j];
		j+=2;
	}

	dev->calibration_data.dig_T1 = raw_cal_full[0];
	dev->calibration_data.dig_T2 = raw_cal_full[1];
	dev->calibration_data.dig_T3 = raw_cal_full[2];
	dev->calibration_data.dig_P1 = raw_cal_full[3];
	dev->calibration_data.dig_P2 = raw_cal_full[4];
	dev->calibration_data.dig_P3 = raw_cal_full[5];
	dev->calibration_data.dig_P4 = raw_cal_full[6];
	dev->calibration_data.dig_P5 = raw_cal_full[7];
	dev->calibration_data.dig_P6 = raw_cal_full[8];
	dev->calibration_data.dig_P7 = raw_cal_full[9];
	dev->calibration_data.dig_P8 = raw_cal_full[10];
	dev->calibration_data.dig_P9 = raw_cal_full[11];

	return HAL_OK;
}







HAL_StatusTypeDef  BMP280_init(BMP280_Handle_t *dev, I2C_HandleTypeDef *hi2c) {

    if (dev == NULL || hi2c == NULL)
    {
        return HAL_ERROR;
    }

    dev->i2c = hi2c;
    dev->state = SENSOR_STATE_INIT;
    dev->sensor = UNKNOWN;
    dev->sensor_address = BMP_BME280_ADDR_SDO_LOW; //need to confirm this. will do below. this is just a primer
    dev->sample_interval_ms = 5000;
    dev->failure_count = 0;
    dev->max_failures = 3;
    dev->data_valid = false;
    dev->initialized = false;

	uint8_t id = 0;



	HAL_I2C_Mem_Read(dev->i2c, dev->sensor_address << 1,
		REG_DEVICE_ID,
		I2C_MEMADD_SIZE_8BIT, &id, 1,
		HAL_MAX_DELAY);

	if(id == BMP280_ID)
	{
		dev->sensor = BMP280;
	}
	else if(id == BME280_ID)
	{
		dev->sensor = BME280;
	}
	else //we did not get the right ID, try to other address.
	{
		HAL_I2C_Mem_Read(dev->i2c, dev->sensor_address << 1,
				REG_DEVICE_ID,
				I2C_MEMADD_SIZE_8BIT, &id, 1,
				HAL_MAX_DELAY);
		if(id == BMP280_ID)
		{
			dev->sensor = BMP280;
		}
		else if(id == BME280_ID)
		{
			dev->sensor = BME280;
		}
		else
		{
			//we tried both addresses and got invalid devices.
			dev->state = SENSOR_STATE_ERROR;
			return HAL_ERROR;
		}
	}

	//Set some default  values to to the config and control measurement registers.
	uint8_t default_carrier = DEFAULT_CONFIG_REG;

	if(HAL_I2C_Mem_Write(dev->i2c,
			dev->sensor_address << 1,
			REG_CONFIG,
			I2C_MEMADD_SIZE_8BIT,
			&default_carrier,
			1,
			HAL_MAX_DELAY) == HAL_ERROR)
	{// we were unsuccessful in setting up a default setting.
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
	}

	default_carrier = DEFAULT_CTRL_MEAS_REG;

	if(HAL_I2C_Mem_Write(dev->i2c,
			dev->sensor_address << 1,
			REG_CTRL_MEAS,
			I2C_MEMADD_SIZE_8BIT,
			&default_carrier,
			1,
			HAL_MAX_DELAY) == HAL_ERROR)
	{// we were unsuccessful in setting up a default setting.
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
	}

	if(BMX280_read_calibration(dev) == HAL_ERROR) // we were not able to get the calibration so data will be junk
	{
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
	}

    dev->state = SENSOR_STATE_READY;
    dev->initialized = true;

    return HAL_OK;
}



static int32_t bmp280_compensate_T_int32(BMP280_Handle_t *dev, int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t) dev->calibration_data.dig_T1 << 1))) * ((int32_t)dev->calibration_data.dig_T2))
					>> 11;
	var2 = (((((adc_T >> 4) - ((int32_t) dev->calibration_data.dig_T1)) * ((adc_T>>4) - ((int32_t)dev->calibration_data.dig_T1)))
			>> 12)*
			((int32_t)dev->calibration_data.dig_T3)) >> 14;
	dev->t_fine = var1 + var2;
	T = (dev->t_fine * 5 + 128) >> 8;
	return T;
}

static int32_t bmp280_compensate_P_int64(BMP280_Handle_t *dev,int32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t) dev->t_fine)
	- 128000;
	var2 = var1 * var1 * (int64_t) dev->calibration_data.dig_P6;
	var2 = var2 + ((var1 * (int64_t) dev->calibration_data.dig_P5) << 17);
	var2 = var2 + (((int64_t) dev->calibration_data.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) dev->calibration_data.dig_P3) >> 8)
			+ ((var1 * (int64_t) dev->calibration_data.dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dev->calibration_data.dig_P1)
			>> 33;
	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t) dev->calibration_data.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) dev->calibration_data.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t) dev->calibration_data.dig_P7) << 4);

	return (int32_t) p;
}

HAL_StatusTypeDef BMP280_read_measurement(BMP280_Handle_t *dev) {
	//reading in a batch so data is not cutoff.
	uint8_t raw_data[6];
	//TODO: error check to see if this succeeds.
	HAL_I2C_Mem_Read(dev->i2c, dev->sensor_address << 1,
	REG_PRESS_DATA_START,
	I2C_MEMADD_SIZE_8BIT, raw_data, 6,
	HAL_MAX_DELAY);

	int32_t temp_raw =
	    ((int32_t)raw_data[3] << 12) |
	    ((int32_t)raw_data[4] << 4)  |
	    ((int32_t)raw_data[5] >> 4);

	dev->data.temperature_c = bmp280_compensate_T_int32(dev, temp_raw) / 100;

	int32_t pressure_raw =
	    ((int32_t)raw_data[0] << 12) |
	    ((int32_t)raw_data[1] << 4)  |
	    ((int32_t)raw_data[2] >> 4);
	dev->data.pressure_pa = bmp280_compensate_P_int64(dev, pressure_raw) / 256;

	return HAL_OK;
}

