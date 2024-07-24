/*
 * uart_sensor_sim.h
 *
 *  Created on: Jul 12, 2024
 *      Author: DeViL
 */

#ifndef INC_UART_SENSOR_SIM_H_
#define INC_UART_SENSOR_SIM_H_

#include "stm32g0xx_hal.h"
#define MAX_BUFFER_SIZE 256


void Start_UART_Reception(UART_HandleTypeDef *huart);
uint8_t Process_UART_Data(void);



#endif /* INC_UART_SENSOR_SIM_H_ */
