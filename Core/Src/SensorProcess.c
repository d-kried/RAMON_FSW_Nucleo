#include "SensorProcess.h"
#include "FlightSoftware.h"
#include "uart_sensor_sim.h"


#define LPF_ALPHA 0.1f  // Low-pass filter coefficient

static I2C_HandleTypeDef *hi2c_1;
static I2C_HandleTypeDef *hi2c_2;
static UART_HandleTypeDef *huart_debug;
static UART_HandleTypeDef *huart_gps;

static char uartBuffer[MAX_BUFFER_SIZE];
static int bufferIndex = 0;
static uint8_t rx_data[1];



HAL_StatusTypeDef SensorProcess_PseudoReadBarometer(void) {
    DataPool* dataPool = DataPool_GetInstance();
    static uint8_t pressureBytes[4];
    static uint8_t byteCount = 0;

    // Try to receive the next byte
    if (byteCount < 4) {
        if (HAL_UART_Receive(huart_debug, &pressureBytes[byteCount], 1, 10) == HAL_OK) {
            Debug_Print("Received byte %d: 0x%02X\r\n", byteCount, pressureBytes[byteCount]);
            byteCount++;
        } else {
            return HAL_BUSY;
        }
    }

    // If we've received all 4 bytes
    if (byteCount == 4) {
        // Convert 4 bytes to float
        float pressure;
        memcpy(&pressure, pressureBytes, sizeof(float));

        // Store in DataPool
        dataPool->rawPressure_Pa = pressure;

        // Reset byte count for next reading
        byteCount = 0;

        Debug_Print("Pseudo pressure reading: %.2f Pa\r\n", pressure);
        return HAL_OK;
    }

    return HAL_BUSY;  // Not all bytes received yet
}






void SensorProcess_Init(I2C_HandleTypeDef *hi2c_ch1, I2C_HandleTypeDef *hi2c_ch2, UART_HandleTypeDef *huart_dbg, UART_HandleTypeDef *huart_gps_param) {
    hi2c_1 = hi2c_ch1;
    hi2c_2 = hi2c_ch2;
    huart_debug = huart_dbg;
    huart_gps = huart_gps_param;

    if (BMP280_Init(hi2c_1, huart_debug) != HAL_OK) {
            Debug_Print("BMP280 initialization unsuccessful!\r\n");
        } else {
        	//Debug_Print("BMP280 initialization successful!\r\n");
        }

    if (LSM6DSOX_Init(hi2c_2) != HAL_OK) {
                Debug_Print("IMU initialization unsuccessful!\r\n");
            } else {
            	//Debug_Print("IMU initialization successful!\r\n");
            }


    NEO6MV2_Init(huart_gps, huart_debug);



/*
    if (SDCard_Init() != HAL_OK) {
            Debug_Print("SD Card initialization failed\r\n");
        } else {
            Debug_Print("SD Card initialized successfully\r\n");
        }
        */
    // Initialize channel 2 when ready
}

void SensorProcess_PseudoInit(UART_HandleTypeDef *huart) {
    huart_debug = huart;
    // Initialize UART for pseudo-sensor data reception
    HAL_UART_Receive_IT(huart_debug, (uint8_t*)rx_data, 1);
    Debug_Print("Pseudo-sensor initialization complete\r\n");
}

HAL_StatusTypeDef SensorProcess_ReadAllSensors(void) {
    HAL_StatusTypeDef status = HAL_OK;

    // Read barometer data

    status = SensorProcess_ReadBarometer();
    if (status != HAL_OK) return status;


    //SensorProcess_PseudoReadBarometer();


    // Read IMU data
    status = SensorProcess_ReadIMU();
    if (status != HAL_OK) return status;

    // Read other sensors as needed
    //status = SensorProcess_ReadGPS();
	//if (status != HAL_OK) return status;


    status = SensorProcess_ReadGPS();
	if (status != HAL_OK) return status;



    return status;
}


HAL_StatusTypeDef SensorProcess_ReadBarometer(void) {
    int32_t temp_c, pressure_hPa;
    DataPool* dataPool = DataPool_GetInstance();

    HAL_StatusTypeDef status = BMP280_ReadData(hi2c_1, huart_debug, &temp_c, &pressure_hPa);
    if (status == HAL_OK) {
        dataPool->rawTemperature_C = (float)temp_c / 100.0f;
        dataPool->rawPressure_Pa = (float)pressure_hPa * 100.0f;
    }


    return status;
}


// Sub-function for reading IMU data
HAL_StatusTypeDef SensorProcess_ReadIMU(void) {
    DataPool* dataPool = DataPool_GetInstance();
    int16_t gyroX, gyroY, gyroZ;
    HAL_StatusTypeDef ret;

    // Read Gyroscope X-axis Data
    ret = LSM6DSOX_ReadGyroX(hi2c_2, &gyroX);
    if (ret != HAL_OK) {
        Debug_Print("Failed to read Gyro X data\r\n");
        return ret;
    }

    // Read Gyroscope Y-axis Data
    ret = LSM6DSOX_ReadGyroY(hi2c_2, &gyroY);
    if (ret != HAL_OK) {
        Debug_Print("Failed to read Gyro Y data\r\n");
        return ret;
    }

    // Read Gyroscope Z-axis Data
    ret = LSM6DSOX_ReadGyroZ(hi2c_2, &gyroZ);
    if (ret != HAL_OK) {
        Debug_Print("Failed to read Gyro Z data\r\n");
        return ret;
    }

    // Convert raw data to dps (degrees per second)
    float gyro_sensitivity = 8.75f / 1000.0f; // 8.75 mdps/LSB to dps/LSB
    dataPool->rawGyroX_dps = gyroX * gyro_sensitivity;
    dataPool->rawGyroY_dps = gyroY * gyro_sensitivity;
    dataPool->rawGyroZ_dps = gyroZ * gyro_sensitivity;

    // Apply bias correction (assuming bias values are stored in the DataPool)
    /*
    dataPool->rawGyroX_dps -= dataPool->gyroBiasX_dps;
    dataPool->rawGyroY_dps -= dataPool->gyroBiasY_dps;
    dataPool->rawGyroZ_dps -= dataPool->gyroBiasZ_dps;
    */
    return HAL_OK;
}



HAL_StatusTypeDef SensorProcess_ReadGPS(void) {
    uint8_t rx_data;
    if (HAL_UART_Receive(huart_gps, &rx_data, 1, 10) == HAL_OK) {
        NEO6MV2_ProcessChar(rx_data);
        return HAL_OK;
    }
    return HAL_TIMEOUT;
}


void SensorProcess_ProcessAllSensors(void) {
    SensorProcess_ProcessBarometer();
    // SensorProcess_ProcessIMU();
    // SensorProcess_ProcessGPS();
    // SensorProcess_ProcessThermo();
}


void SensorProcess_ProcessBarometer(void) {
    DataPool* dataPool = DataPool_GetInstance();

    // Convert pressure to altitude (simplified example)
    float newAltitude = 44330.0f * (1.0f - pow(dataPool->rawPressure_Pa / 101325.0f, 0.1903f));

    // Apply low-pass filter
    dataPool->estimatedAltitude_m = LPF_ALPHA * newAltitude + (1.0f - LPF_ALPHA) * dataPool->estimatedAltitude_m;

    // Calculate vertical speed (simple method)
    static float lastAltitude = 0;
    static uint32_t lastAltitudeTime = 0;
    uint32_t currentTime = HAL_GetTick();
    float timeDelta = (currentTime - lastAltitudeTime) / 1000.0f; // Convert to seconds

    if (timeDelta > 0 && lastAltitudeTime != 0) {
        dataPool->verticalSpeed_mps = (dataPool->estimatedAltitude_m - lastAltitude) / timeDelta;
    }

    lastAltitude = dataPool->estimatedAltitude_m;
    lastAltitudeTime = currentTime;

    // Debug print
    //Debug_Print("Raw Alt: %.2f m, Filtered Alt: %.2f m, Vert Speed: %.2f m/s\r\n",
    //            newAltitude, dataPool->estimatedAltitude_m, dataPool->verticalSpeed_mps);
}
