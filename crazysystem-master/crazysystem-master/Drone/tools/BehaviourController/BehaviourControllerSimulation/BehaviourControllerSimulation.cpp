#include "BehaviourControllerSimulation.h"
#include <math.h>
#include <stdlib.h>
#include <cmath>

float angleOffset = - (M_PI / 2);
int quadrantOffset = 1;
int ticksPerSecond = TICKS_PER_SECOND;

void GoUp(const float height, void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CVector3 vec = {sensors->m_pcPos->GetReading().Position.GetX(),
                    sensors->m_pcPos->GetReading().Position.GetY(), height};
    sensors->m_pcPropellers->SetAbsolutePosition(vec);
}

void GoToGround(CFVector pos, void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CVector3 vec = {pos.x, pos.y, 0};
    sensors->m_pcPropellers->SetAbsolutePosition(vec);
}

float GetDistanceFromGround(void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    return sensors->m_pcPos->GetReading().Position.GetZ();
}

double* GetDistanceFromWalls(void* p, double* wallPositions) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    auto iterDistRead = sensors->m_pcDistance->GetReadingsMap().begin();

    for(size_t i = 0; i < sensors->m_pcDistance->GetReadingsMap().size(); i++) {
        wallPositions[i] = (iterDistRead++)->second;
    }

    return wallPositions;
}

float SetDestination(const CFVector dest, void* p, CFVector direction) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CVector3 castedVector = {dest.x, dest.y, dest.z};
    sensors->m_pcPropellers->SetAbsolutePosition({dest.x, dest.y, dest.z});
    CVector3 diffVector = (sensors->m_pcPos->GetReading().Position - castedVector);
    return diffVector.Length();
}

void SetLinearVelocity(const float speed, CFVector dir, void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CVector3 newDir = {dir.x, dir.y, dir.z};
    newDir *= speed;
    sensors->m_pcPropellers->SetRelativePosition(newDir);
}

CFVector GetPosition(void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CFVector pos = {
        sensors->m_pcPos->GetReading().Position.GetX(),
        sensors->m_pcPos->GetReading().Position.GetY(),
        sensors->m_pcPos->GetReading().Position.GetZ()
    };
    return pos;
}

float GetAngle(void* p, bool withOffset) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CRadians cZAngle, cYAngle, cXAngle;
    sensors->m_pcPos->GetReading().Orientation.ToEulerAngles(cZAngle, cYAngle, cXAngle);

    if(!withOffset) {
        return cZAngle.GetValue();
    }

    return (cZAngle.GetValue() - (M_PI / 2));
}

bool Wait(int* counter, int delay, void* p) {
    int max_iter = TICKS_PER_SECOND * delay;

    if ((*counter) < max_iter) {
        (*counter)++;
        return false;
    }

    (*counter) = 0;
    return true;
}

void Rotate(double radianAngle, void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CRadians rad = CRadians(radianAngle);
    sensors->m_pcPropellers->SetAbsoluteYaw(rad);
}

void Stop(void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    CFVector position = GetPosition(p);
    CVector3 castedPosition = {position.x, position.y, position.z};
    sensors->m_pcPropellers->SetAbsolutePosition(castedPosition);
}

CFVector GetLeftPosition(CFReturnToBase params, void* p) {
    SSensors* sensors = reinterpret_cast<SSensors*>(p);
    float orientation = GetAngle(p, false) + (M_PI / 2) * (params.currentSensor);
    CVector3 dir = {0, 0, 0};
    dir[0] += cos(orientation);
    dir[1] += sin(orientation);
    return {dir[0], dir[1], sensors->m_pcPos->GetReading().Position.GetZ()};
}

void Print(char* str) {
    LOG << str << std::endl;
}


Commander SetCommander() {
    struct Commander commander = {
        angleOffset,
        quadrantOffset,
        ticksPerSecond,
        &GoUp,
        &GoToGround,
        &GetDistanceFromGround,
        &GetDistanceFromWalls,
        &SetDestination,
        &SetLinearVelocity,
        &GetPosition,
        &GetAngle,
        &Wait,
        &Rotate,
        &Stop,
        &GetLeftPosition,
        &Print
    };

    return commander;
}
