/*
 * FlightCriteria.h
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */
#ifndef FLIGHTCRITERIA_H
#define FLIGHTCRITERIA_H

#include <stdint.h>
#include "FlightSoftware.h"


// Forward declaration of the struct
typedef struct FlightCriteria FlightCriteria;

typedef struct FlightCriteria {
	//detect launch
	uint32_t groundStabilizationTime_ms;
	float launchVerticalSpeed_mps;
	float launchAltitudeDelta_m;

	//transition to ascent
	uint32_t launchToAscentDelay_ms;

	//transition to cruise
	float ascentVerticalSpeedDeviationSquareThreshold_m;

	//detect balloonburst
	float balloonBurstVerticalSpeed_mps;
	float balloonBurstAcceleration_mps2;

	//detect landing
	float landingAcceleration_mps2;
	float landingVerticalSpeed_mps;

	//stop recordSD
	float descentStopSDWriteAltitude_m;


/*
    float launchAltitudeDelta_m;
    float launchAcceleration_mps2;
    float launchCountdownTime_s;
    float launchStableSpeedDelta_mps;
    float ascentRate_mps;
    float ascentAltitudeDeviationThreshold_m;
    float ascentVerticalSpeedThreshold_mps;
    uint32_t ascentStabilityDuration_s;
    float cruiseAltitude_m;
    float cruiseVerticalSpeed_mps;
    float descentRate_mps;
    float descentStopSDWriteAltitude_m;
    float landingAltitudeDelta_m;
    float landingVerticalSpeed_mps;
    float balloonBurstVerticalSpeed_mps;
    uint8_t balloonBurstDetectionWindow;
    uint32_t groundStabilizationTime_ms;
    uint32_t launchToAscentDelay_ms;
    float launchVerticalSpeed_mps;
    */
} FlightCriteria;

extern FlightCriteria* g_flightCriteria;

void FlightCriteria_Initialize(void);

#endif // FLIGHTCRITERIA_H
