/*
 * FlightCriteria.c
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */


#include "FlightCriteria.h"


FlightCriteria g_flightCriteriaInstance;
FlightCriteria* g_flightCriteria = &g_flightCriteriaInstance;


void FlightCriteria_Initialize(void) {
	//from ground to launch
	g_flightCriteria->groundStabilizationTime_ms = 60000;	//in ms
	g_flightCriteria->launchVerticalSpeed_mps = 1.0f;
	g_flightCriteria->launchAltitudeDelta_m = 30.0f;

	//from launch to ascent
	g_flightCriteria->launchToAscentDelay_ms = 30000;		//in ms

	//from ascent to cruise
	g_flightCriteria->ascentVerticalSpeedDeviationSquareThreshold_m = 0.5f;


	//from asecent/cruise to descent
	g_flightCriteria->balloonBurstVerticalSpeed_mps = -3.0f;
	g_flightCriteria->balloonBurstAcceleration_mps2 = -10.0f;

	//from descent to landing
	g_flightCriteria->landingAcceleration_mps2 = 10.0f;
	g_flightCriteria->landingVerticalSpeed_mps = 0.5f;
	g_flightCriteria->descentStopSDWriteAltitude_m = 1000.0f;

	/*
	  // Set default values for flight criteria
	    g_flightCriteria->launchAltitudeDelta_m = 10.0f;
	    g_flightCriteria->launchAcceleration_mps2 = 2.0f;
	    g_flightCriteria->launchCountdownTime_s = 10.0f;
	    g_flightCriteria->launchStableSpeedDelta_mps = 0.5f;
	    g_flightCriteria->ascentRate_mps = 5.0f;
	    g_flightCriteria->ascentAltitudeDeviationThreshold_m = 50.0f;
	    g_flightCriteria->ascentVerticalSpeedThreshold_mps = 2.0f;
	    g_flightCriteria->ascentStabilityDuration_s = 30;
	    g_flightCriteria->cruiseAltitude_m = 30000.0f;
	    g_flightCriteria->cruiseVerticalSpeed_mps = 0.5f;
	    g_flightCriteria->descentRate_mps = -5.0f;
	    g_flightCriteria->descentStopSDWriteAltitude_m = 1000.0f;
	    g_flightCriteria->landingAltitudeDelta_m = 5.0f;
	    g_flightCriteria->landingVerticalSpeed_mps = 0.5f;
	    g_flightCriteria->balloonBurstVerticalSpeed_mps = -10.0f;
	    g_flightCriteria->balloonBurstDetectionWindow = 5;
	    g_flightCriteria->groundStabilizationTime_ms = 60000;
	    g_flightCriteria->launchToAscentDelay_ms = 5000;
	    g_flightCriteria->launchVerticalSpeed_mps = 1.0f;
*/
	    // You might want to add some debug output here
	    Debug_Print("Flight Criteria Initialized\r\n");
}

// Additional functions to get or set individual criteria could be added here
