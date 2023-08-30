#ifndef BEHAVIOUR_CONTROLLER_SIMULATION_H
#define BEHAVIOUR_CONTROLLER_SIMULATION_H

#include "../BehaviourController/BehaviourController.h"
#include <iostream>
#include <argos3/core/utility/math/vector3.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>

using namespace argos;

#define TICKS_PER_SECOND 20

/**
 * Struct for all the differents sensors that the simulation uses
 */
typedef struct SSensors {
    CCI_CrazyflieDistanceScannerSensor* m_pcDistance;
    CCI_QuadRotorPositionActuator *m_pcPropellers;
    CCI_PositioningSensor *m_pcPos;
    CCI_BatterySensor* m_pcBattery;
    CCI_RangeAndBearingActuator *m_pcP2PSend;
    CCI_RangeAndBearingSensor *m_pcP2PReceive;
} SSensors;

/*
 * The following methods are part of the commander for the drones in the simulation.
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
 * Sets the commander that the simulation needs
 */
Commander SetCommander();

#endif
