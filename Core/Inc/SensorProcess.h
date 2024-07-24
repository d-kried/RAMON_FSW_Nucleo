#ifndef INC_SENSORPROCESS_H_
#define INC_SENSORPROCESS_H_

// library
#include "stm32g0xx_hal.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

//sensor implement
#include "BMP280.h"
#include "LSM6DSOX.h"
#include "NEO6MV2.h"

//
#include "DataPool.h"
#include "uart_sensor_sim.h"

/*
// Struct to hold sensor data
typedef struct {
	int32_t lastAltitude;
    uint32_t lastAltitudeTime;
    // Add other sensor data fields as needed
} SensorData;
*/

// GPS Buffer Size
#define GPS_BUFFER_SIZE 256


// Function prototypes
void SensorProcess_Init(I2C_HandleTypeDef *hi2c_ch1, I2C_HandleTypeDef *hi2c_ch2, UART_HandleTypeDef *huart_debug, UART_HandleTypeDef *huart_gps);
HAL_StatusTypeDef SensorProcess_ReadAllSensors(void);
void SensorProcess_ProcessAllSensors(void);

void SensorProcess_ProcessBarometer(void);
void SensorProcess_ProcessIMU(void);
void SensorProcess_ProcessGPS(void);
void SensorProcess_ProcessThermo(void);

HAL_StatusTypeDef SensorProcess_ReadBarometer(void);
HAL_StatusTypeDef SensorProcess_ReadIMU(void);
HAL_StatusTypeDef SensorProcess_ReadGPS(void);

void SensorProcess_GPS_ProcessChar(char rx_char);
void SensorProcess_GPS_ParseNMEA(char* sentence);

#endif /* INC_SENSORPROCESS_H_ */


