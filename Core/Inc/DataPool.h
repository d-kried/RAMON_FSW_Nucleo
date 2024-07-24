/*
 * DataPool.h
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */
// DataPool.h

#ifndef DATAPOOL_H
#define DATAPOOL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Ground,
    Launch,
    Ascent,
    Cruise,
    Descent,
    Landing
} FlightPhase;

typedef struct {

    // Raw sensor readings
    float rawPressure_Pa;
    float rawTemperature_C;
    float rawAccelX_mps2, rawAccelY_mps2, rawAccelZ_mps2;
    float rawGyroX_dps, rawGyroY_dps, rawGyroZ_dps;
    int gpsTime;
	double rawGPSLat;
	double rawGPSLon;
	float rawGPSAlt_m;

    // Processed data
    float estimatedAltitude_m;
    float verticalSpeed_mps;
    float totalAcceleration_mps2;

    // Flight state
    FlightPhase currentPhase;
    float launchSiteAltitude_m;
    uint32_t missionStartTime_ms;

    // Event flags
    bool Flag_LaunchEvent;
    bool Flag_AscentEvent;
    bool Flag_CruiseEvent;
    bool Flag_BalloonBurstEvent;
    bool Flag_SDcardOFFEvent;
    bool Flag_LandingEvent;
} DataPool;

void DataPool_Init(void);
DataPool* DataPool_GetInstance(void);
void DataPool_SaveState(void);
void DataPool_LoadState(void);
bool DataPool_IsInitialized(void);  // New function to check initialization state

#endif // DATAPOOL_H
