/*
 * NEO6MV2.h
 *
 *  Created on: Jul 20, 2024
 *      Author: DeViL
 */

#ifndef INC_NEO6MV2_H_
#define INC_NEO6MV2_H_

#include "stm32g0xx_hal.h"
#include <stdbool.h>

#define BUFFER_SIZE 256


typedef struct {
    int time;
    double latitude;
    double longitude;
    float altitude;
} GPS_Data;


void NEO6MV2_Init(UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *huart_debug);
void NEO6MV2_ProcessChar(char rx_char);
void NEO6MV2_ParseNMEA(char* sentence);

#endif /* INC_NEO6MV2_H_ */
