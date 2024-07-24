#include "NEO6MV2.h"
#include "DataPool.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define GPS_BUFFER_SIZE 256

static UART_HandleTypeDef *gps_uart;
static UART_HandleTypeDef *debug_uart;
static char rxBuffer[GPS_BUFFER_SIZE];
static int rxIndex = 0;

void NEO6MV2_Init(UART_HandleTypeDef *huart_gps, UART_HandleTypeDef *huart_debug) {
    gps_uart = huart_gps;
    debug_uart = huart_debug;
    rxIndex = 0;
    memset(rxBuffer, 0, GPS_BUFFER_SIZE);
    HAL_UART_Transmit(debug_uart, (uint8_t*)"GPS Module Initialized\r\n", 24, HAL_MAX_DELAY);
}

void NEO6MV2_ProcessChar(char rx_char) {
    if (rx_char == '$') {
        rxIndex = 0;
    }

    if (rxIndex < GPS_BUFFER_SIZE - 1) {
        rxBuffer[rxIndex++] = rx_char;

        if (rx_char == '\n' || rx_char == '\r') {
            rxBuffer[rxIndex] = '\0';
            NEO6MV2_ParseNMEA(rxBuffer);
            rxIndex = 0;
        }
    } else {
        rxIndex = 0;
    }
}

static double convertToDecimalDegrees(const char* coord, char dir) {
    double degrees = atof(coord) / 100.0;
    double minutes = degrees - (int)degrees;
    degrees = (int)degrees + (minutes * 100.0 / 60.0);
    return (dir == 'S' || dir == 'W') ? -degrees : degrees;
}

void NEO6MV2_ParseNMEA(char* sentence) {
    if (strncmp(sentence, "$GPGGA", 6) == 0) {
        char *token = strtok(sentence, ",");
        int field = 0;
        char time[20] = "", lat[20] = "", lat_dir[2] = "", lon[20] = "", lon_dir[2] = "", alt[20] = "";
        while (token != NULL) {
            switch(field) {
                case 1: strncpy(time, token, sizeof(time) - 1); break;
                case 2: strncpy(lat, token, sizeof(lat) - 1); break;
                case 3: strncpy(lat_dir, token, sizeof(lat_dir) - 1); break;
                case 4: strncpy(lon, token, sizeof(lon) - 1); break;
                case 5: strncpy(lon_dir, token, sizeof(lon_dir) - 1); break;
                case 9: strncpy(alt, token, sizeof(alt) - 1); break;
            }
            token = strtok(NULL, ",");
            field++;
        }

        // Get DataPool instance
        DataPool* dataPool = DataPool_GetInstance();

        // Update DataPool with parsed GPS data
        dataPool->gpsTime = atoi(time);
        dataPool->rawGPSLat = convertToDecimalDegrees(lat, lat_dir[0]);
        dataPool->rawGPSLon = convertToDecimalDegrees(lon, lon_dir[0]);
        dataPool->rawGPSAlt_m = atof(alt);

        int hours, minutes, seconds;
        hours = dataPool->gpsTime / 10000;
        minutes = (dataPool->gpsTime % 10000) / 100;
        seconds = dataPool->gpsTime % 100;

        char message[100];
        snprintf(message, sizeof(message), "GPS Time: %02d:%02d:%02d, Lat: %.6f, Lon: %.6f, Alt: %.2f\r\n",
                 hours, minutes, seconds, dataPool->rawGPSLat, dataPool->rawGPSLon, dataPool->rawGPSAlt_m);
        HAL_UART_Transmit(debug_uart, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
    }
}
