#include <qmc5883L.h>







HAL_StatusTypeDef  qmc5883l_init(QMC5883L_Handle_t *dev, I2C_HandleTypeDef *hi2c)
{

    if (dev == NULL)
    {
    	return HAL_ERROR;
    }


    dev->i2c = hi2c;
    dev->state = SENSOR_STATE_INIT;
    dev->sample_interval_ms = 250;
    dev->failure_count = 0;
    dev->max_failures = 3;
    dev->measurement.valid = false;
    dev->initialized = false;
    dev->calibrated = false;

    if (hi2c == NULL)
	{
		dev->failure_count++;
		dev->state = SENSOR_STATE_ERROR;
		return HAL_ERROR;
	}

    if(HAL_I2C_IsDeviceReady(dev->i2c, QMC5883L_ADDR << 1, 2, 100) != HAL_OK)
    {
    	dev->state = SENSOR_STATE_ERROR;
    	return HAL_ERROR;
    }

    dev->address = QMC5883L_ADDR;
    //we could read back from 0x0D to confirm chip ID, expect 0xFF
    // if we get here, we have the right device on the line.


    uint8_t default_carrier = 0x01;

    //need to write to set reset reg for continuous mode
    if(HAL_I2C_Mem_Write(dev->i2c,
    			dev->address << 1,
				REG_QMC_SET_RESET,
    			I2C_MEMADD_SIZE_8BIT,
				&default_carrier,
    			1,
    			HAL_MAX_DELAY)!= HAL_OK)
    {//could not write to set reset reg
    	dev->state = SENSOR_STATE_ERROR;
    	return HAL_ERROR;
    }

    //let's send the default configurations to the control registers
    default_carrier = QMC5883L_DEFAULT_CONTROL_1;

    if(HAL_I2C_Mem_Write(dev->i2c,
    			dev->address << 1,
				REG_QMC_CONTROL_1,
    			I2C_MEMADD_SIZE_8BIT,
				&default_carrier,
    			1,
    			HAL_MAX_DELAY) != HAL_OK)
    {//could not write to control reg 1
    	dev->state = SENSOR_STATE_ERROR;
    	return HAL_ERROR;
    }

    default_carrier = QMC5883L_DEFAULT_CONTROL_2;

    if(HAL_I2C_Mem_Write(dev->i2c,
    			dev->address << 1,
				REG_QMC_CONTROL_2,
    			I2C_MEMADD_SIZE_8BIT,
				&default_carrier,
    			1,
    			HAL_MAX_DELAY)!= HAL_OK)
    {//could not write to control reg 2
    	dev->state = SENSOR_STATE_ERROR;
    	return HAL_ERROR;
    }



    dev->state = SENSOR_STATE_READY;
    dev->initialized = true;



	return HAL_OK;
}






HAL_StatusTypeDef  qmc5883l_read_measurement(QMC5883L_Handle_t *dev)
{

    //lets get some data
    uint8_t raw_values[6];
    //batch read
    if(HAL_I2C_Mem_Read(dev->i2c,
    			dev->address << 1,
				REG_QMC_DATA_X_LSB,
    			I2C_MEMADD_SIZE_8BIT,
				raw_values,
    			6,
    			HAL_MAX_DELAY) != HAL_OK)
    {//we were not able to read data from the device
    	dev->state = SENSOR_STATE_ERROR;
    	dev->measurement.valid = false;
    	return HAL_ERROR;
    }

    dev->measurement.valid = true;

	dev->measurement.raw_x = (int16_t)((raw_values[1] << 8) | raw_values[0]);
    dev->measurement.raw_y = (int16_t)((raw_values[3] << 8) | raw_values[2]);
    dev->measurement.raw_z = (int16_t)((raw_values[5] << 8) | raw_values[4]);



	return HAL_OK;
}

HAL_StatusTypeDef qmc5883l_calibrate(QMC5883L_Handle_t *dev, uint32_t duration_ms)
{
    if (dev == NULL || dev->initialized == false)
    {
        return HAL_ERROR;
    }

    int16_t min_x = INT16_MAX;
    int16_t min_y = INT16_MAX;
    int16_t min_z = INT16_MAX;

    int16_t max_x = INT16_MIN;
    int16_t max_y = INT16_MIN;
    int16_t max_z = INT16_MIN;

    uint32_t start_time = HAL_GetTick();
    bool got_sample = false;

    while ((HAL_GetTick() - start_time) < duration_ms)
    {
        if (qmc5883l_read_measurement(dev) == HAL_OK &&
        		dev->measurement.valid == true)
        {
            int16_t x = dev->measurement.raw_x;
            int16_t y = dev->measurement.raw_y;
            int16_t z = dev->measurement.raw_z;

            if (x < min_x) min_x = x;
            if (x > max_x) max_x = x;

            if (y < min_y) min_y = y;
            if (y > max_y) max_y = y;

            if (z < min_z) min_z = z;
            if (z > max_z) max_z = z;

            got_sample = true;
        }

        HAL_Delay(20);
    }

    if (got_sample == false)
    {
        dev->measurement.valid = false;
        return HAL_ERROR;
    }

    dev->measurement.offset_x = (int16_t)(((int32_t)max_x + (int32_t)min_x) / 2);
    dev->measurement.offset_y = (int16_t)(((int32_t)max_y + (int32_t)min_y) / 2);
    dev->measurement.offset_z = (int16_t)(((int32_t)max_z + (int32_t)min_z) / 2);

    dev->measurement.cal_x = dev->measurement.raw_x - dev->measurement.offset_x;
    dev->measurement.cal_y = dev->measurement.raw_y - dev->measurement.offset_y;
    dev->measurement.cal_z = dev->measurement.raw_z - dev->measurement.offset_z;

    dev->calibrated = true;

    return HAL_OK;
}


HAL_StatusTypeDef qmc5883l_update_heading(QMC5883L_Handle_t *dev)
{
    if (dev == NULL || dev->initialized == false)
    {
        return HAL_ERROR;
    }

    if (qmc5883l_read_measurement(dev) != HAL_OK)
    {
        dev->measurement.valid = false;
        return HAL_ERROR;
    }

    if (dev->measurement.valid == false)
    {
        return HAL_ERROR;
    }

    dev->measurement.cal_x = dev->measurement.raw_x - dev->measurement.offset_x;
    dev->measurement.cal_y = dev->measurement.raw_y - dev->measurement.offset_y;
    dev->measurement.cal_z = dev->measurement.raw_z - dev->measurement.offset_z;

    float x = (float)dev->measurement.cal_x;
    float y = (float)dev->measurement.cal_y;
    float z = (float)dev->measurement.cal_z;

    dev->measurement.gauss_x = x;
    dev->measurement.gauss_y = y;
    dev->measurement.gauss_z = z;

    float heading_rad = atan2f(y, x);
    float heading_deg = heading_rad * (180.0f / 3.14159265f);

    if (heading_deg < 0.0f)
    {
        heading_deg += 360.0f;
    }

    dev->measurement.heading_deg = heading_deg;
    dev->measurement.timestamp_ms = HAL_GetTick();
    dev->measurement.valid = true;

    return HAL_OK;
}
