#include "PseudoSensor.h"
#include "FlightSoftware.h" // for Debug_Print
#include <string.h>
#include <stdlib.h>
#include <float.h>

static PseudoSensorContext g_context;

#define VALUES_COUNT 3
#define BYTES_PER_VALUE 8 // 8 bytes for a double
#define FRAME_START_BYTE 0xAA
#define FRAME_END_BYTE 0x55

// Enable float support for printf
#ifdef __GNUC__
#define PRINTF_FLOAT_SUPPORT
#endif

// Function prototypes
static void ProcessReceivedData(void);
static bool IsValidData(double* values);

// Function to swap endianness of a double
double swap_endian_double(double val) {
    double ret;
    char *dst = (char*)&ret;
    char *src = (char*)&val;

    for(int i = 0; i < 8; i++) {
        dst[i] = src[7-i];
    }

    return ret;
}

void PseudoSensor_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == g_context.huart->Instance) {
        uint8_t receivedByte = (uint8_t)(huart->Instance->RDR & 0xFF);

        switch (g_context.rxState) {
            case WAITING_FOR_START:
                if (receivedByte == FRAME_START_BYTE) {
                    g_context.rxState = RECEIVING_DATA;
                    g_context.rxIndex = 0;
                }
                break;

            case RECEIVING_DATA:
                g_context.rxBuffer[g_context.rxIndex++] = receivedByte;
                if (g_context.rxIndex >= VALUES_COUNT * BYTES_PER_VALUE) {
                    g_context.rxState = WAITING_FOR_END;
                }
                break;

            case WAITING_FOR_END:
                if (receivedByte == FRAME_END_BYTE) {
                    ProcessReceivedData();
                }
                g_context.rxState = WAITING_FOR_START;
                break;
        }
    }

    // Restart UART reception
    HAL_UART_Receive_IT(huart, (uint8_t*)&(huart->Instance->RDR), 1);
}

HAL_StatusTypeDef PseudoSensor_Init(UART_HandleTypeDef *huart) {
    g_context.huart = huart;
    g_context.rxIndex = 0;
    g_context.dataReady = 0;
    memset(g_context.rxBuffer, 0, PSEUDO_SENSOR_BUFFER_SIZE);

    // Configure UART
    huart->Instance->CR1 |= USART_CR1_RXNEIE_RXFNEIE;  // Enable RXNE interrupt
    HAL_NVIC_EnableIRQ(USART2_IRQn);  // Enable USART2 interrupt (adjust if using a different UART)

    Debug_Print("Pseudo-sensor initialized. Type 'help' for commands.\r\n");
    return HAL_OK;
}

static bool IsValidData(double* values) {
    bool pressureValid = (isfinite(values[0]) && values[0] >= 30000 && values[0] <= 120000);
    bool temperatureValid = (isfinite(values[1]) && values[1] >= -100 && values[1] <= 100);
    bool altitudeValid = (isfinite(values[2]) && values[2] >= -1000 && values[2] <= 100000);

    Debug_Print("Validation results:\n");
    Debug_Print("Pressure (%.2f Pa): %s\n", values[0], pressureValid ? "Valid" : "Invalid");
    Debug_Print("Temperature (%.2f °C): %s\n", values[1], temperatureValid ? "Valid" : "Invalid");
    Debug_Print("Altitude (%.2f m): %s\n", values[2], altitudeValid ? "Valid" : "Invalid");

    return pressureValid && temperatureValid && altitudeValid;
}

static void ProcessReceivedData(void) {
    double values[VALUES_COUNT];
    memcpy(values, g_context.rxBuffer, sizeof(values));

    // Debug: Print raw bytes
    Debug_Print("Raw bytes: ");
    for (int i = 0; i < VALUES_COUNT * BYTES_PER_VALUE; i++) {
        Debug_Print("%02X ", g_context.rxBuffer[i]);
    }
    Debug_Print("\r\n");

    // Debug: Print values as received
    Debug_Print("Values as received: P=%.2f, T=%.2f, A=%.2f\r\n", values[0], values[1], values[2]);

    if (IsValidData(values)) {
        DataPool* dataPool = DataPool_GetInstance();
        dataPool->rawPressure_Pa = values[0];
        dataPool->rawTemperature_C = values[1];
        dataPool->estimatedAltitude_m = values[2];

        Debug_Print("Received valid data: P=%.2f, T=%.2f, A=%.2f\r\n",
                    dataPool->rawPressure_Pa,
                    dataPool->rawTemperature_C,
                    dataPool->estimatedAltitude_m);
    } else {
        Debug_Print("Received invalid data: P=%.2f, T=%.2f, A=%.2f\r\n", values[0], values[1], values[2]);
    }
}

void PseudoSensor_HandleCommand(char* command) {
    // ... (rest of the function remains the same)
}

HAL_StatusTypeDef PseudoSensor_ReadData(void) {
    // ... (rest of the function remains the same)
    return HAL_OK;
}

HAL_StatusTypeDef PseudoSensor_ProcessData(DataPool *dataPool) {
    // ... (rest of the function remains the same)
    return HAL_OK;
}
