/*
 * StateMachine.h
 *
 *  Created on: Jul 10, 2024
 *      Author: DeViL
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

//library
#include <stdbool.h>
#include <stdint.h>

#include "DataPool.h"
#include "FlightCriteria.h"
#include "MemoryManagement.h"


void StateMachine_Ground(void);
void StateMachine_Launch(void);
void StateMachine_Ascent(void);
void StateMachine_Cruise(void);
void StateMachine_Descent(void);
void StateMachine_Landing(void);
void StateMachine_Init(void);

void StateMachine_HandleState(void);

bool StateMachine_CalibrateSensors(void);
bool StateMachine_CheckLaunchCondition(void);
bool StateMachine_IsAscentStable(void);
bool StateMachine_DetectBalloonBurst(void);
bool StateMachine_CheckLandingCondition(void);
bool StateMachine_IsVerticalSpeedStable(void);
/*
void StateMachine_HandleState(void);
bool StateMachine_CalibrateSensors(void);
bool StateMachine_CheckLaunchCondition(void);
bool StateMachine_IsAscentStable(void);
bool StateMachine_DetectBalloonBurst(void);
bool StateMachine_CheckLandingCondition(void);
void StateMachine_UpdateVerticalSpeedBuffer(float speed);
bool StateMachine_IsVerticalSpeedStable(void);

// Individual phase function prototypes
void StateMachine_Ground(void);
void StateMachine_Launch(void);
void StateMachine_Ascent(void);
void StateMachine_Cruise(void);
void StateMachine_Descent(void);
void StateMachine_Landing(void);

// helper
static void updateConfidenceCounter(bool condition);
static bool isConfident();
static void resetConfidenceCounter();
static void blinkLed();
*/

#endif // STATEMACHINE_H
