/*
 * BMP280.h
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_


#include "stm32g0xx_hal.h"




// Function declarations
HAL_StatusTypeDef BMP280_Init(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);
HAL_StatusTypeDef BMP280_ReadData(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart, int32_t *temp_c, int32_t *pressure_hPa);
void I2C_Scan(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);






#endif /* INC_BMP280_H_ */
