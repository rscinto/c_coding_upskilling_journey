//0b00000000  OR 0b01010101
//01010101
//
//0b11111111 OR 0b01010101
//11111111
//
//0b00000000  &  0b01010101
//00000000
//
//0b11111111 & 0b01010101
//01010101
//
//shifting left
//00000001
//00000010
//00000100
//00001000
//00010000
//00100000
//01000000
//
//shifting right
//10000000
//01000000
//00100000
//00010000
//00001000
//00000100
//00000010
//00000001
//
//00010000
//SET 4th bit.
//00000000
//REMOVE 4th bit.
//
//Big to little endian conversion.
//10101011110011011110111100000001
//Big Endian: ABCDEF01
//00000001111011111100110110101011
//Little Endian: 01EFCDAB
//
//0b00000000  OR 0b01010101
//01010101
//
//0b11111111 OR 0b01010101
//11111111
//
//0b00000000  &  0b01010101
//00000000
//
//0b11111111 & 0b01010101
//01010101
//
//shifting left
//00000001
//00000010
//00000100
//00001000
//00010000
//00100000
//01000000
//
//shifting right
//10000000
//01000000
//00100000
//00010000
//00001000
//00000100
//00000010
//00000001
//
//00010000
//SET 4th bit.
//00000000
//REMOVE 4th bit.
//
//Big to little endian conversion.
//10101011110011011110111100000001
//Big Endian: ABCDEF01
//00000001111011111100110110101011
//Little Endian: 01EFCDAB
//
//0b00000000  OR 0b01010101
//01010101
//
//0b11111111 OR 0b01010101
//11111111
//
//0b00000000  &  0b01010101
//00000000
//
//0b11111111 & 0b01010101
//01010101
//
//shifting left
//00000001
//00000010
//00000100
//00001000
//00010000
//00100000
//01000000
//
//shifting right
//10000000
//01000000
//00100000
//00010000
//00001000
//00000100
//00000010
//00000001
//
//00010000
//SET 4th bit.
//00000000
//REMOVE 4th bit.
//
//Big to little endian conversion.
//10101011110011011110111100000001
//Big Endian: ABCDEF01
//00000001111011111100110110101011
//Little Endian: 01EFCDAB
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
/* USER CODE END Includes */

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}


void print_binary_uint8(uint8_t value)
{
    for (int i = 7; i >= 0; i--)
    {
        printf("%d", (value >> i) & 1);
    }

    printf("\r\n");
}

void print_binary_uint32(uint32_t value)
{
    for (int i = 31; i >= 0; i--)
    {
        printf("%d", (value >> i) & 1);
    }

    printf("\r\n");
}

int main()
{
    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        uint8_t data =        0b00000001;
        uint8_t data_mix =    0b01010101;
        uint8_t data_ones =   0b11111111;
        uint8_t data_zeroes = 0b00000000;
        channels = data_zeroes | data_mix;
        printf("0b00000000  OR 0b01010101\r\n");
        print_binary_uint8(channels);
        printf("\r\n");
        HAL_Delay(500);

        channels = data_ones | data_mix;
        printf("0b11111111 OR 0b01010101\r\n");
        print_binary_uint8(channels);
        printf("\r\n");
        HAL_Delay(500);

        channels = data_zeroes & data_mix;
        printf("0b00000000  &  0b01010101\r\n");
        print_binary_uint8(channels);
        printf("\r\n");
        HAL_Delay(500);

        channels = data_ones & data_mix;
        printf("0b11111111 & 0b01010101\r\n");
        print_binary_uint8(channels);
        printf("\r\n");
        HAL_Delay(500);

        printf("shifting left\r\n");
        for(int i = 0; i < 7; i++)
        {
            print_binary_uint8(data);
            data = data << 1;
            HAL_Delay(500);
        }
        printf("\r\n");

        printf("shifting right\r\n");
        for(int i = 0; i < 8; i++)
        {
            print_binary_uint8(data);
            data = data >> 1;
            HAL_Delay(500);
        }
        printf("\r\n");

        data = 0b00000000;
        data |= (1 << 4); //setting fourth bit
        print_binary_uint8(data);
        printf("SET 4th bit.\r\n");
        HAL_Delay(1000);

        data &= ~(1 << 4); //remove fourth bit
        print_binary_uint8(data);
        printf("REMOVE 4th bit.\r\n");
        HAL_Delay(1000);
        printf("\r\n");

        uint32_t value = 0xABCDEF01;
        printf("Big to little endian conversion.\r\n");
        uint32_t swapped =
            ((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) << 8 ) |
            ((value & 0x00FF0000) >> 8 ) |
            ((value & 0xFF000000) >> 24);
        print_binary_uint32(value);
        printf("Big Endian: %08lX\r\n", value);
        print_binary_uint32(swapped);
        printf("Little Endian: %08lX\r\n", swapped);
        HAL_Delay(1000);
        printf("\r\n");

    }
    /* USER CODE END 3 */
}