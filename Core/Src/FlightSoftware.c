#include <FlightSoftware.h>
#include "MemoryManagement.h"
#include "PseudoSensor.h"
#include "SensorProcess.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


// Global variables for I2C and UART handles
I2C_HandleTypeDef *g_hi2c_1;
I2C_HandleTypeDef *g_hi2c_2;
UART_HandleTypeDef *g_huart_debug;
UART_HandleTypeDef *g_huart_gps;

HAL_StatusTypeDef FlightSoftware_Init(I2C_HandleTypeDef *hi2c_ch1, I2C_HandleTypeDef *hi2c_ch2,
                                      UART_HandleTypeDef *huart_debug, UART_HandleTypeDef *huart_gps) {
    g_hi2c_1 = hi2c_ch1;
    g_hi2c_2 = hi2c_ch2;
    g_huart_debug = huart_debug;
    g_huart_gps = huart_gps;

	/*
    // Initialize hardware
#ifdef USE_PSEUDO_SENSORS
    SensorProcess_PseudoInit(g_huart_debug);
    Debug_Print("Flight Software in PSEUDO MODE\r\n")
#else
    SensorProcess_Init(g_hi2c_1, g_hi2c_2, g_huart_debug);
#endif
*/

    // Initialize DataPool
    DataPool_Init();

    // Load the saved state
    FlightSoftware_LoadState();

    // Initialize memory management
    MemoryManager_Initialize();

    // Initialize flight criteria
    FlightCriteria_Initialize();

    // Initialize Sensor
    SensorProcess_Init(g_hi2c_1, g_hi2c_2, g_huart_debug, g_huart_gps);

    // Initialize state machine
    StateMachine_Init();

    Debug_Print("Flight Software Initialized\r\n");



}

void FlightSoftware_Run(void) {
    Debug_Print("Entering FlightSoftware_Run loop\r\n");




    while (1) {
    	//DataPool* dataPool = DataPool_GetInstance();

    	//Debug_Print("Flight Software Read\r\n");
    	SensorProcess_ReadAllSensors();

    	//Debug_Print("Flight Software Process\r\n");
    	SensorProcess_ProcessAllSensors();
    	//Debug_Print("Flight Software handleState\r\n");
    	StateMachine_HandleState();

    	HAL_Delay(1000);


    }





    /*
    while (1) {
        HAL_StatusTypeDef status = PseudoSensor_ReadData();
        if (status == HAL_OK) {
            if (PseudoSensor_ProcessData(dataPool) == HAL_OK) {
                // Data successfully processed, now handle it
                StateMachine_HandleState();
            }
        } else if (status != HAL_TIMEOUT) {
            Debug_Print("Error reading UART: %d\r\n", status);
        }

        // Other non-blocking operations can go here

        HAL_Delay(1); // Small delay to prevent busy-waiting
    }
    */

}

void FlightSoftware_LoadState(void) {
    // This function will load the saved state from non-volatile memory
    // For now, we'll just call DataPool_LoadState()
    DataPool_LoadState();

    // In the future, you might also load StateMachine and FlightCriteria states
    Debug_Print("Flight State Loaded\r\n");


    //char buf[50];
    //sprintf(buf, );
    //HAL_UART_Transmit(huart_debug, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}

void FlightSoftware_SaveState(void) {
    // This function will save the current state to non-volatile memory
    // For now, we'll just call DataPool_SaveState()
    DataPool_SaveState();

    // In the future, you might also save StateMachine and FlightCriteria states
    Debug_Print("Flight State Saved\r\n");


    //char buf[50];
    //sprintf(buf, "Flight State Saved\r\n");
    //HAL_UART_Transmit(huart_debug, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}

void FlightSoftware_InitMemoryManagement(void) {
    // This function will initialize the memory management system
    // For now, it's just a placeholder
    // In the future, you might initialize SD card writing, etc.

	Debug_Print("Memory Management Initialized\r\n");
    //char buf[50];
    //sprintf(buf, "Memory Management Initialized\r\n");
    //HAL_UART_Transmit(huart_debug, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
}



void Debug_Print(const char* format, ...) {

	  char buffer[256];
	    va_list args;
	    va_start(args, format);
	    vsnprintf(buffer, sizeof(buffer), format, args);
	    va_end(args);

	    HAL_UART_Transmit(g_huart_debug, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);

}

