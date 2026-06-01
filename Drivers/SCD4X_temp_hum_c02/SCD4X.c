#include "scd4x.h"

static I2C_HandleTypeDef *scd4x_i2c;


uint8_t start_periodic_measurement_CMD[2] = {0x21, 0xB1};
uint8_t read_measurement_CMD[2] = {0xec, 0x05};
uint8_t stop_periodic_measurement_CMD[2] = {0x3f, 0x86};


//Provided function written by SENSIRION
static uint8_t SCD4X_generate_crc(uint8_t msb, uint8_t lsb) {
	uint8_t data[2] = {msb, lsb};
	uint16_t current_byte;
	uint8_t crc = CRC8_INIT;
	uint8_t crc_bit;
	/* calculates 8-Bit checksum with given polynomial */
	for (current_byte = 0; current_byte < 2; ++current_byte) {
		crc ^= (data[current_byte]);
		for (crc_bit = 8; crc_bit > 0; --crc_bit) {
			if (crc & 0x80)
				crc = (crc << 1) ^ CRC8_POLYNOMIAL;
			else
				crc = (crc << 1);
		}
	}
	return crc;
}






static bool SCD4X_crc_ok(uint8_t msb, uint8_t lsb, uint8_t received_crc)
{
	return (received_crc) == SCD4X_generate_crc(msb,lsb);
}






static HAL_StatusTypeDef SCD4X_send_command(uint16_t command)
{
    uint8_t cmd[2];

    cmd[0] = command >> 8;      // MSB
    cmd[1] = command & 0xFF;    // LSB

    return HAL_I2C_Master_Transmit(
        scd4x_i2c,
        SCD4X_ADDR << 1,
        cmd,
        2,
        HAL_MAX_DELAY
    );
}

void SCD4X_init(I2C_HandleTypeDef *hi2c) {
	//power on
	//wait 1000mS
	scd4x_i2c = hi2c;
	HAL_Delay(1000);
	SCD4X_stop_periodic_measurement();
    HAL_Delay(500);
    SCD4X_start_periodic_measurement();
}

HAL_StatusTypeDef  SCD4X_start_periodic_measurement(){
	return SCD4X_send_command(SCD4X_CMD_START_PERIODIC_MEASUREMENT);
}






HAL_StatusTypeDef SCD4X_read_measurement(SCD4X_Measurement_t *out)
{

    if (out == NULL)
    {
        return HAL_ERROR;
    }

    uint8_t rx[9];

    HAL_StatusTypeDef status;

    status = SCD4X_send_command(SCD4X_CMD_READ_MEASUREMENT);
    if (status != HAL_OK)
    {
        return status;
    }

    HAL_Delay(1);

    status = HAL_I2C_Master_Receive(
        scd4x_i2c,
        SCD4X_ADDR << 1,
        rx,
        9,
        HAL_MAX_DELAY
    );

    if (status != HAL_OK)
    {
        return status;
    }


    if(!SCD4X_crc_ok( rx[0], rx[1],  rx[2]))
    {
    	return HAL_ERROR;
    }

    if(!SCD4X_crc_ok( rx[3], rx[4],  rx[5]))
    {
    	return HAL_ERROR;
    }

    if(!SCD4X_crc_ok( rx[6], rx[7],  rx[8]))
    {
    	return HAL_ERROR;
    }


    //promote uint8 to uint16. fill with MSB. shift left 8 bits. fill lower 8 bits with LSB by using OR operatation
    uint16_t raw_co2  = ((uint16_t)rx[0] << 8) | rx[1];
    uint16_t raw_temp = ((uint16_t)rx[3] << 8) | rx[4];
    uint16_t raw_rh   = ((uint16_t)rx[6] << 8) | rx[7];

    out->co2_ppm = raw_co2;
    out->temperature_c = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
    out->humidity_rh = 100.0f * ((float)raw_rh / 65535.0f);

    return HAL_OK;
}





HAL_StatusTypeDef  SCD4X_stop_periodic_measurement(){
	return SCD4X_send_command(SCD4X_CMD_STOP_PERIODIC_MEASUREMENT);
}











