#ifndef PSEUDO_SENSOR_H
#define PSEUDO_SENSOR_H

#include "stm32g0xx_hal.h"
#include "DataPool.h"

#define PSEUDO_SENSOR_BUFFER_SIZE 256

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t rxBuffer[PSEUDO_SENSOR_BUFFER_SIZE];
    uint16_t rxIndex;
    uint8_t dataReady;
    enum {WAITING_FOR_START, RECEIVING_DATA, WAITING_FOR_END} rxState;
} PseudoSensorContext;

HAL_StatusTypeDef PseudoSensor_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef PseudoSensor_ReadData(void);
HAL_StatusTypeDef PseudoSensor_ProcessData(DataPool *dataPool);
void PseudoSensor_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void PseudoSensor_HandleCommand(char* command);

#endif // PSEUDO_SENSOR_H
