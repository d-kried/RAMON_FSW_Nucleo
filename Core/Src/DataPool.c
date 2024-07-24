/*
 * DataPool.c
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */
// DataPool.c

#include "DataPool.h"
#include <string.h>

static DataPool s_dataPoolInstance;
static bool s_isDataPoolInitialized = false;

void DataPool_Init(void) {
    if (!s_isDataPoolInitialized) {
        memset(&s_dataPoolInstance, 0, sizeof(DataPool));
        s_dataPoolInstance.currentPhase = Ground;
        s_isDataPoolInitialized = true;
    }
    // Any additional initialization that should happen every time, even if already initialized
}

DataPool* DataPool_GetInstance(void) {
    if (!s_isDataPoolInitialized) {
        DataPool_Init();
    }
    return &s_dataPoolInstance;
}

void DataPool_SaveState(void) {
    // Implementation depends on your specific storage method
    // This could involve writing to EEPROM or flash memory
}

void DataPool_LoadState(void) {
    // Implementation depends on your specific storage method
    // This could involve reading from EEPROM or flash memory
    s_isDataPoolInitialized = true;  // Mark as initialized after loading state
}

bool DataPool_IsInitialized(void) {
    return s_isDataPoolInitialized;
}
