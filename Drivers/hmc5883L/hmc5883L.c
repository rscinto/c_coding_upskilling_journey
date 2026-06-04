#include "hmc5883L.h"



static HAL_StatusTypeDef try_address(HMC5883L_Handle_t *dev, uint8_t candidate_address)
{
    static uint8_t id[3] = {0xAA, 0xBB, 0xCC};
    HAL_StatusTypeDef status;

    HAL_StatusTypeDef ready_hmc ;
    HAL_StatusTypeDef ready_qmc  ;

    ready_hmc = HAL_I2C_IsDeviceReady(dev->i2c, 0x1E << 1, 2, 100);
    ready_qmc = HAL_I2C_IsDeviceReady(dev->i2c, 0x0D << 1, 2, 100);


    status = HAL_I2C_Mem_Read(
        dev->i2c,
        candidate_address << 1,
        REG_HMC5883L_ID_A,
        I2C_MEMADD_SIZE_8BIT,
        id,
        3,
        HAL_MAX_DELAY
    );


    return status;
	/*
	uint8_t id[3];

	if(candidate_address == HMC5883L_ADDR)
	{
		if(	HAL_I2C_Mem_Read(dev->i2c,
				candidate_address << 1,
				REG_HMC5883L_ID_A,
			I2C_MEMADD_SIZE_8BIT, id, 3,
			HAL_MAX_DELAY) != HAL_OK)
		{
			// we were not able to successfully talk to the chip
			return HAL_ERROR; // error is too deep, don't update sensor status or failure count
		}
	}
	else if(candidate_address == QMC5883L_ADDR)
	{
		if(	HAL_I2C_Mem_Read(dev->i2c,
				candidate_address << 1,
				0x0D,
			I2C_MEMADD_SIZE_8BIT, id, 1,
			HAL_MAX_DELAY) != HAL_OK)
		{
			// we were not able to successfully talk to the chip
			return HAL_ERROR; // error is too deep, don't update sensor status or failure count
		}
	}

	return HAL_OK;
	*/
}




HAL_StatusTypeDef  hmc5883l_init(HMC5883L_Handle_t *dev, I2C_HandleTypeDef *hi2c)
{

    if (dev == NULL)
    {
    	return HAL_ERROR;
    }
    else if (hi2c == NULL)
    {
		dev->failure_count++;
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
    }

    dev->i2c = hi2c;
    dev->state = SENSOR_STATE_INIT;
    //dev->sensor = UNKNOWN;
    dev->sample_interval_ms = 5000;
    dev->failure_count = 0;
    dev->max_failures = 3;
    dev->data_valid = false;
    dev->initialized = false;

    //#define HMC5883L_ADDR              0x1E
    //#define QMC5883L_ADDR              0x0D

    //check for valid ID and device
    if (try_address(dev, HMC5883L_ADDR) == HAL_OK)
    {
        // valid BMX280 found
    }
    else if (try_address(dev, QMC5883L_ADDR) == HAL_OK)
    {
        // valid BMX280 found
    }
    else
    {
		dev->failure_count++;
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
    }


	return HAL_OK;
}






HAL_StatusTypeDef  hmc5883l_read_measurement(HMC5883L_Handle_t *dev)
{


	return HAL_OK;
}
