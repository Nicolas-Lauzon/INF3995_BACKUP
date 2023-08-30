#ifndef BEHAVIOUR_CONTROLLER_DRONE_H
#define BEHAVIOUR_CONTROLLER_DRONE_H

#include <stdlib.h>

#include "../BehaviourController/BehaviourController.h"

#include "app.h"
#include "param.h"
#include "log.h"
#include "config.h"
#include "radiolink.h"
#include "FreeRTOS.h"
#include "task.h"
#include "configblock.h"
#include "commander.h"

#define HEIGHT_STEP 0.06f
#define SETPOINT_PRIORITY 3
#define MIN_HEIGHT 0.02f
#define METER_IN_MM 1000.0f
#define DESTINATION_SPEED 0.1
#define REVERSE_SPEED_OFFSET 3.0f
#define ACCELERATION_FACTOR 2.5
#define HALF_CIRCLE_DEGREES 180
#define TICKS_PER_SECOND 4
#define MM_TO_CM 10.0f

/**
 * sets the hover point for the drone (sets direction, speed, yaw)
 * @param setpoint the desired setpoint
 * @param vx the speed in x
 * @param vy the speed in y
 * @param z the speed in z
 * @param yawrate the "turn" rate
 */
void setHoverSetpoint(setpoint_t* setpoint, float vx, float vy, float z, float yawrate);

/*
 * Following methods are part of the commander for the drone.
 * Their description is given in BehaviorController.h
*/

void GoUp(const float height, void* p);

void GoToGround(CFVector pos, void* p);

float GetDistanceFromGround(void* p);

double* GetDistanceFromWalls(void* p, double* wallPositions);

float SetDestination(const CFVector dest, void* p, CFVector direction);

void SetLinearVelocity(const float speed, CFVector dir, void* p);

CFVector GetPosition(void* p);

float GetAngle(void* p, bool withOffset);

bool Wait(int* counter, int delay, void* p);

void Rotate(double radianAngle, void* p);

void Stop(void* p);

CFVector GetLeftPosition(CFReturnToBase params, void* p);

void Print(char* str);

/**
 * Sets the commander that the drone needs
 */
Commander SetCommander();

#endif