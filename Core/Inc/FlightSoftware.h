#ifndef INC_FLIGHTSOFTWARE_H_
#define INC_FLIGHTSOFTWARE_H_


#include "stm32g0xx_hal.h"
#include "DataPool.h"
#include "SensorProcess.h"
#include "StateMachine.h"


// #include "FlightCriteria.h"
// #include "MemoryManagement.h"

#define SENSOR_READ_INTERVAL 10 // 10 ms (100 Hz)
#define SENSOR_PROCESS_INTERVAL 50 // 50 ms (20 Hz)
#define DEBUG_PRINT_INTERVAL 1000 // 1000 ms (1 Hz)


// Initialization function
HAL_StatusTypeDef FlightSoftware_Init(I2C_HandleTypeDef *hi2c_ch1, I2C_HandleTypeDef *hi2c_ch2,
                                      UART_HandleTypeDef *huart_debug, UART_HandleTypeDef *huart_gps);


// Main loop function
void FlightSoftware_Run(void);

// Helper functions
void FlightSoftware_LoadState(void);
void FlightSoftware_SaveState(void);
void FlightSoftware_InitMemoryManagement(void);

// Debug print function declaration
void Debug_Print(const char* format, ...);



#endif /* INC_FLIGHTSOFTWARE_H_ */
