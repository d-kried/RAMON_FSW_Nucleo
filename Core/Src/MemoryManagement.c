#include "MemoryManagement.h"

static FATFS fs;
static FIL fil;

HAL_StatusTypeDef MemoryManager_Initialize(void) {
    FRESULT fres;

    // Initialize FATFS
    if (MX_FATFS_Init() != APP_OK) {
        Debug_Print("Failed to initialize FATFS\r\n");
        return HAL_ERROR;
    }
    Debug_Print("Memory Management Initialized1\r\n");
    HAL_Delay(1000); // Wait for SD card to initialize


    fres = f_mount(&fs, "", 0);
    if (fres != FR_OK) {
        Debug_Print("Failed to mount SD card\r\n");
        return HAL_ERROR;
    }

    Debug_Print("Memory Management Initialized2\r\n");
    return HAL_OK;
}


HAL_StatusTypeDef MemoryManager_WriteToSDCard(const char* filename, const char* data, bool allowWrite) {
    if (!allowWrite) {
        return HAL_OK;  // Silently succeed if writing is not allowed
    }

    FRESULT fres;
    UINT bytesWritten;
    FIL fil;

    fres = f_open(&fil, filename, FA_WRITE | FA_OPEN_ALWAYS | FA_OPEN_APPEND);
    if (fres != FR_OK) {
        Debug_Print("Failed to open file %s for writing\r\n", filename);
        return HAL_ERROR;
    }

    fres = f_write(&fil, data, strlen(data), &bytesWritten);
    if (fres != FR_OK) {
        Debug_Print("Failed to write to file %s\r\n", filename);
        f_close(&fil);
        return HAL_ERROR;
    }

    f_close(&fil);
    return HAL_OK;
}

void MemoryManager_FlushAndCloseSDCard(void) {
    f_close(&fil);
    f_mount(NULL, "", 0);
    Debug_Print("SD card flushed and closed\r\n");
}

bool MemoryManager_ReopenSDCard(void) {
    FRESULT fres = f_mount(&fs, "", 0);
    if (fres != FR_OK) {
        Debug_Print("Failed to remount SD card\r\n");
        return false;
    }
    return true;
}

void MemoryManager_WriteDataPoolToSDCard(void) {
    FRESULT fres;
    UINT bytesWritten;
    char buffer[512];
    DataPool* dataPool = DataPool_GetInstance();

    fres = f_open(&fil, "datapool.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fres != FR_OK) {
        Debug_Print("Failed to open datapool file for writing\r\n");
        return;
    }

    snprintf(buffer, sizeof(buffer),
             "EstAlt: %.2f\nVertSpeed: %.2f\nRawTemp: %.2f\nRawPress: %.2f\nPhase: %d\n",
             dataPool->estimatedAltitude_m, dataPool->verticalSpeed_mps,
             dataPool->rawTemperature_C, dataPool->rawPressure_Pa,
             dataPool->currentPhase);

    fres = f_write(&fil, buffer, strlen(buffer), &bytesWritten);
    if (fres != FR_OK) {
        Debug_Print("Failed to write datapool to file\r\n");
    }

    f_close(&fil);
}

// Placeholder implementations for SRAM operations
// These would need to be expanded based on your specific SRAM setup
/*
static char sram_buffer[4096]; // Simulated SRAM buffer
static int sram_index = 0;

void MemoryManager_WriteToSRAM(void) {
    DataPool* dataPool = DataPool_GetInstance();
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer), "T:%lu,A:%.2f,V:%.2f\n",
                       HAL_GetTick(), dataPool->estimatedAltitude_m, dataPool->verticalSpeed_mps);

    if (sram_index + len < sizeof(sram_buffer)) {
        memcpy(sram_buffer + sram_index, buffer, len);
        sram_index += len;
    } else {
        Debug_Print("SRAM buffer full\r\n");
    }
}

void MemoryManager_TransferSRAMToSDCard(void) {
    FRESULT fres;
    UINT bytesWritten;

    fres = f_open(&fil, "sram_data.txt", FA_WRITE | FA_CREATE_ALWAYS);
    if (fres != FR_OK) {
        Debug_Print("Failed to open SRAM data file for writing\r\n");
        return;
    }

    fres = f_write(&fil, sram_buffer, sram_index, &bytesWritten);
    if (fres != FR_OK) {
        Debug_Print("Failed to write SRAM data to file\r\n");
    } else {
        Debug_Print("Transferred %d bytes from SRAM to SD card\r\n", bytesWritten);
    }

    f_close(&fil);
    sram_index = 0; // Reset SRAM buffer
}
*/
