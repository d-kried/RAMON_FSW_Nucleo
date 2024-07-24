#include "Statemachine.h"
#include "SensorProcess.h"
#include "MemoryManagement.h"
#include "FlightCriteria.h"
#include "FlightSoftware.h" // For Debug_Print
#include <math.h>
#include <stdio.h>

// Enable float support for printf
#ifdef __GNUC__
#define PRINTF_FLOAT_SUPPORT
#endif

#define CONFIDENCE_THRESHOLD 5
#define LED_BLINK_INTERVAL 500 // ms
#define LOG_INTERVAL 1000 // 1 second interval for logging
#define LOG_FILENAME "flight_log.txt"
#define MAX_LOG_SIZE 512
#define STABILITY_WINDOW 5

static FlightPhase currentPhase = Ground;
//static uint32_t phaseStartTime = 0;
//static uint32_t lastLedToggleTime = 0;
static uint8_t confidenceCounter = 0;
static uint32_t lastLogTime = 0;

static void updateConfidenceCounter(bool condition);
static bool isConfident(void);
static void resetConfidenceCounter(void);
static bool isStabilityWindowStable(float* dataWindow, int windowSize, float threshold);


//static void writeFlightLog(void);
//static void blinkLed(void);
//static void turnOffLed(void);

void StateMachine_Init(void) {
    currentPhase = Ground;
    //phaseStartTime = HAL_GetTick();
    lastLogTime = 0;
    Debug_Print("State Machine Initialized\r\n");
}

void StateMachine_HandleState(void) {
    DataPool* data = DataPool_GetInstance();

    if (!DataPool_IsInitialized()) {
        DataPool_Init();
        currentPhase = Ground;
        data->missionStartTime_ms = HAL_GetTick();
    } else if (data->currentPhase != Ground) {
        currentPhase = data->currentPhase;
    }

    // Write flight log at regular intervals
        uint32_t currentTime = HAL_GetTick();
        if (currentTime - lastLogTime >= LOG_INTERVAL) {
            writeFlightLog();
            lastLogTime = currentTime;
        }

    switch (currentPhase) {
        case Ground:
            StateMachine_Ground();
            break;
        case Launch:
            StateMachine_Launch();
            break;
        case Ascent:
            StateMachine_Ascent();
            break;
        case Cruise:
            StateMachine_Cruise();
            break;
        case Descent:
            StateMachine_Descent();
            break;
        case Landing:
            StateMachine_Landing();
            break;
    }

}


void StateMachine_Ground(void) {
    DataPool* data = DataPool_GetInstance();
    static uint32_t stabilizationStartTime = 0;
    static float altitudeSum = 0;
    static int altitudeCount = 0;
    static bool calibrationComplete = false;

    if (stabilizationStartTime == 0) {
        stabilizationStartTime = HAL_GetTick();
        Debug_Print("Starting ground stabilization period\r\n");
    }

    uint32_t elapsedTime = HAL_GetTick() - stabilizationStartTime;
    if (elapsedTime < g_flightCriteria->groundStabilizationTime_ms) {
        //blinkLed(); // Indicate system not ready
        altitudeSum += data->estimatedAltitude_m;
        altitudeCount++;
    } else if (elapsedTime == g_flightCriteria->groundStabilizationTime_ms){
    	data->launchSiteAltitude_m = altitudeSum / altitudeCount;
    	Debug_Print("Launch site altitude set to: %.2f m\n", data->launchSiteAltitude_m);
    } else if (!calibrationComplete && StateMachine_CalibrateSensors()) {
        calibrationComplete = true;
        //turnOffLed(); // Turn off LED after calibration
        Debug_Print("Sensor calibration complete\r\n");
    } else if (calibrationComplete && StateMachine_CheckLaunchCondition()) {
        data->currentPhase = Launch;
        data->missionStartTime_ms = HAL_GetTick();
        resetConfidenceCounter();
        Debug_Print("Launch event detected. Transitioning to Launch phase\r\n");
    }
}


void StateMachine_Launch(void) {
    DataPool* data = DataPool_GetInstance();
    uint32_t launchDuration = HAL_GetTick() - data->missionStartTime_ms;

    if (launchDuration > g_flightCriteria->launchToAscentDelay_ms) {
    	data->currentPhase = Ascent;
        resetConfidenceCounter();
        Debug_Print("Ascent event detected. Transitioning to Ascent phase\r\n");
    } else if (data->estimatedAltitude_m - data->launchSiteAltitude_m > 3000.0f) { // 3km failsafe
        currentPhase = Ascent;
        Debug_Print("Failsafe: Transitioning to Ascent phase\r\n");
    }
}

void StateMachine_Ascent(void) {
    DataPool* data = DataPool_GetInstance();

    if (StateMachine_IsAscentStable()) {
        data->currentPhase = Cruise;
        data->Flag_CruiseEvent = true;
        resetConfidenceCounter();
        Debug_Print("Cruise event detected. Transitioning to Cruise phase\r\n");
    }

    if (StateMachine_DetectBalloonBurst()) {
        data->currentPhase = Descent;
        resetConfidenceCounter();
        Debug_Print("Balloon burst event detected. Transitioning to Descent phase\r\n");
    }
}

void StateMachine_Cruise(void) {
	DataPool* data = DataPool_GetInstance();
	if (StateMachine_DetectBalloonBurst()) {
	        data->currentPhase = Descent;
	        resetConfidenceCounter();
	        Debug_Print("Balloon burst event detected. Transitioning to Descent phase\r\n");
	    }
}

void StateMachine_Descent(void) {
    DataPool* data = DataPool_GetInstance();

    if (data->estimatedAltitude_m <= g_flightCriteria->descentStopSDWriteAltitude_m && data->Flag_BalloonBurstEvent && !(data->Flag_SDcardOFFEvent)) {
        data->Flag_SDcardOFFEvent = true;
        MemoryManager_FlushAndCloseSDCard();
        Debug_Print("Switched to RAM-only logging at altitude: %.2f m\r\n", data->estimatedAltitude_m);
    }

    if (StateMachine_CheckLandingCondition()) {
        data->currentPhase = Landing;
        resetConfidenceCounter();
        Debug_Print("Landing event detected. Transitioning to Landing phase\r\n");
    }
}

void StateMachine_Landing(void) {


}
void writeFlightLog(void) {
    DataPool* data = DataPool_GetInstance();
    char logBuffer[MAX_LOG_SIZE];
    uint32_t currentTime = HAL_GetTick() / 1000; // Time in seconds

    snprintf(logBuffer, sizeof(logBuffer),
        "%lu,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.6f,%.6f,%.2f\r\n",
        currentTime,
        data->currentPhase,
        data->estimatedAltitude_m,
        data->rawTemperature_C,
        data->rawPressure_Pa,
        data->verticalSpeed_mps,
        data->rawAccelX_mps2,
        data->rawAccelY_mps2,
        data->rawAccelZ_mps2,
        data->rawGyroX_dps,
        data->rawGyroY_dps,
        data->rawGyroZ_dps,
        data->gpsTime,
        data->rawGPSLat,
        data->rawGPSLon,
        data->rawGPSAlt_m);

    // Determine if SD card writing is allowed
    bool allowSDWrite = !data->Flag_SDcardOFFEvent;

    // Write to SD card
    HAL_StatusTypeDef status = MemoryManager_WriteToSDCard(LOG_FILENAME, logBuffer, allowSDWrite);
    if (status != HAL_OK) {
        Debug_Print("Failed to write flight log to SD card\r\n");
    }

    // Optionally, you can also print the log to debug output
    Debug_Print("Flight Log: %s", logBuffer);
}

bool StateMachine_CalibrateSensors(void) {
    // Implement sensor calibration logic here
    return true; // Placeholder
}

bool StateMachine_CheckLaunchCondition(void) {
    DataPool* data = DataPool_GetInstance();
    float altitudeDelta = data->estimatedAltitude_m - data->launchSiteAltitude_m;

    bool altitudeCondition = altitudeDelta > g_flightCriteria->launchAltitudeDelta_m;
    bool speedCondition = data->verticalSpeed_mps > g_flightCriteria->launchVerticalSpeed_mps;

    updateConfidenceCounter(altitudeCondition && speedCondition);

    if (isConfident()) {
        data->Flag_LaunchEvent = true;
        return true;
    }
    return false;
}

bool StateMachine_IsAscentStable(void) {
    DataPool* data = DataPool_GetInstance();

    static float previousSpeeds[STABILITY_WINDOW] = { 0 };
    static int speedIndex = 0;
    static int sampleCountSpeed = 0;

    // Add current speed and altitude to the circular buffers
    previousSpeeds[speedIndex] = data->verticalSpeed_mps;
    speedIndex = (speedIndex + 1) % STABILITY_WINDOW;

    if (sampleCountSpeed < STABILITY_WINDOW) {
        sampleCountSpeed++;
    }
    // Only start checking for stability once we have enough samples
    if (sampleCountSpeed == STABILITY_WINDOW) {
        bool isSpeedStable = isStabilityWindowStable(previousSpeeds, STABILITY_WINDOW, g_flightCriteria->ascentVerticalSpeedDeviationSquareThreshold_m);
        // Return true only if both speed is stable
        return isSpeedStable;
    }
    // Not enough samples yet
    return false;
}

bool StateMachine_DetectBalloonBurst(void) {
    DataPool* data = DataPool_GetInstance();
    updateConfidenceCounter(data->verticalSpeed_mps < g_flightCriteria->balloonBurstVerticalSpeed_mps || data->rawAccelZ_mps2 < g_flightCriteria->balloonBurstAcceleration_mps2);
    if (isConfident()) {
        data->Flag_BalloonBurstEvent = true;
        return true;
    }
    return false;
}

bool StateMachine_CheckLandingCondition(void) {
    DataPool* data = DataPool_GetInstance();
    updateConfidenceCounter(fabs(data->verticalSpeed_mps) < g_flightCriteria->landingVerticalSpeed_mps || fabs(data->rawAccelZ_mps2) > g_flightCriteria->landingAcceleration_mps2);
    if (isConfident()) {
        data->Flag_LandingEvent = true;
        return true;
    }
    return false;
}



static void updateConfidenceCounter(bool condition) {
    if (condition) {
        confidenceCounter++;
    } else {
        confidenceCounter = 0;
    }
}

static bool isConfident(void) {
    return confidenceCounter >= CONFIDENCE_THRESHOLD;
}

static void resetConfidenceCounter(void) {
    confidenceCounter = 0;
}

bool isStabilityWindowStable(float* dataWindow, int windowSize, float threshold) {
    // Initialize the result to true
    bool allBelowThreshold = true;

    // Check each element in the data window
    for (int i = 0; i < windowSize; i++) {
        if (dataWindow[i] <= threshold) {
            allBelowThreshold = false;
            break; // Exit early if any element is not below the threshold
        }
    }

    // Check if the variance is below the threshold
    return (allBelowThreshold);
}

/*
static void blinkLed(void) {
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastLedToggleTime >= LED_BLINK_INTERVAL) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_GREEN);
        lastLedToggleTime = currentTime;
    }
}

static void turnOffLed(void) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_GREEN, GPIO_PIN_RESET);
}
*/
