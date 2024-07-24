/*
 * BMP280.c
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */

#include "BMP280.h"
#include <stdio.h>
#include <string.h>

#define BMP280_REG_TEMP_MSB 0xFA
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_CONFIG 0xF5
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CHIP_ID 0xD0
#define BMP280_CHIP_ID 0x58

static const uint8_t BMP280_ADDR = 0x76 << 1;
static int32_t t_fine;
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

static uint8_t buf[50];

HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart2) {
    uint8_t chip_id = 0;
    HAL_StatusTypeDef ret;

    // Read chip ID
    ret = HAL_I2C_Mem_Read(hi2c, BMP280_ADDR, BMP280_REG_CHIP_ID, I2C_MEMADD_SIZE_8BIT, &chip_id, 1, HAL_MAX_DELAY);

    /* - to clear terminal feed, ignore this already working
    if (ret != HAL_OK) {
        sprintf((char*)buf, "Error reading chip ID: %d\r\n", ret);
        HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
        //return ret;
    }

    sprintf((char*)buf, "Read chip ID: 0x%02X\r\n", chip_id);
    HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

    if (chip_id != BMP280_CHIP_ID) {
        sprintf((char*)buf, "Incorrect chip ID: 0x%02X, expected 0x%02X\r\n", chip_id, BMP280_CHIP_ID);
        HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
        //return HAL_ERROR;
    }

    sprintf((char*)buf, "BMP280 found! Chip ID: 0x%02X\r\n", chip_id);
    HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
*/


    // Read calibration data
    uint8_t cal_data[24];
    ret = HAL_I2C_Mem_Read(hi2c, BMP280_ADDR, 0x88, I2C_MEMADD_SIZE_8BIT, cal_data, 24, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        sprintf((char*)buf, "Error reading calibration data\r\n");
        HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
        return ret;
    }

    // Parse calibration data
    dig_T1 = (cal_data[1] << 8) | cal_data[0];
    dig_T2 = (cal_data[3] << 8) | cal_data[2];
    dig_T3 = (cal_data[5] << 8) | cal_data[4];
    dig_P1 = (cal_data[7] << 8) | cal_data[6];
    dig_P2 = (cal_data[9] << 8) | cal_data[8];
    dig_P3 = (cal_data[11] << 8) | cal_data[10];
    dig_P4 = (cal_data[13] << 8) | cal_data[12];
    dig_P5 = (cal_data[15] << 8) | cal_data[14];
    dig_P6 = (cal_data[17] << 8) | cal_data[16];
    dig_P7 = (cal_data[19] << 8) | cal_data[18];
    dig_P8 = (cal_data[21] << 8) | cal_data[20];
    dig_P9 = (cal_data[23] << 8) | cal_data[22];

    // Configure the sensor
    uint8_t config = 0x00;  // No IIR filter
    ret = HAL_I2C_Mem_Write(hi2c, BMP280_ADDR, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        sprintf((char*)buf, "Error configuring sensor\r\n");
        HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
        return ret;
    }

    uint8_t ctrl_meas = 0x27;  // Temperature and pressure oversampling x1, normal mode
    ret = HAL_I2C_Mem_Write(hi2c, BMP280_ADDR, BMP280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        sprintf((char*)buf, "Error setting measurement control\r\n");
        HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
        return ret;
    }

    sprintf((char*)buf, "BMP280 initialized successfully!\r\n");
    HAL_UART_Transmit(huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);

    return HAL_OK;
}


HAL_StatusTypeDef BMP280_ReadData(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart, int32_t *temp_c, int32_t *pressure_hPa) {
    uint8_t data[6];
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(hi2c, BMP280_ADDR, BMP280_REG_PRESS_MSB, I2C_MEMADD_SIZE_8BIT, data, 6, HAL_MAX_DELAY);

    if (ret != HAL_OK) {
        sprintf((char*)buf, "Error reading sensor data: %d\r\n", ret);
        HAL_UART_Transmit(huart, buf, strlen((char*)buf), HAL_MAX_DELAY);
        return ret;  // Return the error status instead of void
    }

		  	  int32_t adc_P = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
		  	  int32_t adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);

		  	      // Compensate temperature
		  	      int32_t var1, var2;
		  	      var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
		  	      var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
		  	      t_fine = var1 + var2;
		  	    *temp_c = (t_fine * 5 + 128) >> 8;
		  	      //int32_t temp_c = (t_fine * 5 + 128) >> 8;

		  	      // Compensate pressure (simplified, might need 64-bit variables for full accuracy)
		  	      int32_t p;
		  	      var1 = (((int32_t)t_fine) >> 1) - 64000;
		  	      var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)dig_P6);
		  	      var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
		  	      var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
		  	      var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)dig_P2) * var1) >> 1)) >> 18;
		  	      var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
		  	      if (var1 == 0) {
		  	          p = 0;  // Avoid exception caused by division by zero
		  	      } else {
		  	          p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
		  	          if (p < 0x80000000) {
		  	              p = (p << 1) / ((uint32_t)var1);
		  	          } else {
		  	              p = (p / (uint32_t)var1) * 2;
		  	          }
		  	          var1 = (((int32_t)dig_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
		  	          var2 = (((int32_t)(p >> 2)) * ((int32_t)dig_P8)) >> 13;
		  	          p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
		  	      }
		  	    *pressure_hPa = p / 100;

		  	      //int32_t pressure_hPa = p / 100;
		  	    return HAL_OK;
}

void I2C_Scan(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart) {
    char msg[64];
    sprintf(msg, "Scanning I2C bus:\r\n");
    HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    for (uint8_t i = 1; i < 128; i++) {
        HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i << 1), 2, 10);
        if (result == HAL_OK) {
            sprintf(msg, "Device found at address 0x%02X\r\n", i);
            HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
}
