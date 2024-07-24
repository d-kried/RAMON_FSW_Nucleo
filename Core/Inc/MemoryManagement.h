/*
 * MemoryManagement.h
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */

#ifndef INC_MEMORYMANAGEMENT_H_
#define INC_MEMORYMANAGEMENT_H_

#include "stm32g0xx_hal.h"
#include "app_fatfs.h"
#include "fatfs_sd.h"

#include <stdbool.h>
#include <stdint.h>

#include "MemoryManagement.h"
#include "FlightSoftware.h"

HAL_StatusTypeDef MemoryManager_Initialize(void);
//void MemoryManager_WriteToSDCard(void);
void MemoryManager_FlushAndCloseSDCard(void);
bool MemoryManager_ReopenSDCard(void);

// Function to write current DataPool state to SD card
HAL_StatusTypeDef MemoryManager_WriteToSDCard(const char* filename, const char* data, bool allowWrite);


// Function to write to SRAM when SD card writing is not possible
void MemoryManager_WriteToSRAM(void);

// Function to transfer SRAM data to SD card when possible
void MemoryManager_TransferSRAMToSDCard(void);

#endif /* INC_MEMORYMANAGEMENT_H_ */
