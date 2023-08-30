#include "BehaviourControllerDrone.h"

float angleOffset = 0;
int quadrantOffset = 0;
int ticksPerSecond = TICKS_PER_SECOND;

float altitude = 0.0f;
float yaw = 0.0f;

void setHoverSetpoint(setpoint_t* setpoint, float vx, float vy, float z, float yaw) {
    setpoint->mode.z = modeAbs;
    setpoint->position.z = z;
    setpoint->mode.yaw = modeAbs;
    setpoint->attitude.yaw = yaw;
    setpoint->mode.x = modeVelocity;
    setpoint->mode.y = modeVelocity;
    setpoint->velocity.x = vx;
    setpoint->velocity.y = vy;
    setpoint->velocity_body = true;
}

void GoUp(const float height, void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    float ground = GetDistanceFromGround(NULL);

    if(ground < height) {
        ground += HEIGHT_STEP;
    }

    setHoverSetpoint(setpoint, 0, 0, ground, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    altitude = height;
}

void GoToGround(CFVector pos, void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    float ground = GetDistanceFromGround(NULL);

    if(ground > MIN_HEIGHT) {
        ground -= HEIGHT_STEP;
    }

    setHoverSetpoint(setpoint, 0, 0, ground, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    altitude = 0.0f;
}

float GetDistanceFromGround(void* p) {
    logVarId_t logIdPosZ = logGetVarId("range", "zrange");
    return logGetFloat(logIdPosZ) / METER_IN_MM;
}

double* GetDistanceFromWalls(void* p, double* wallPositions) {
    static double walls[4];
    walls[0] = logGetFloat(logGetVarId("range", "front")) / MM_TO_CM;
    walls[1] = logGetFloat(logGetVarId("range", "left")) / MM_TO_CM;
    walls[2] = logGetFloat(logGetVarId("range", "back")) / MM_TO_CM;
    walls[3] = logGetFloat(logGetVarId("range", "right")) / MM_TO_CM;
    return walls;
}

float SetDestination(const CFVector dest, void* p, CFVector direction) {
    SetLinearVelocity(DESTINATION_SPEED, direction, p);
    return 0.0f;
}

void SetLinearVelocity(const float speed, CFVector dir, void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    float newSpeed = speed;

    if (newSpeed < 0.0f) {
        newSpeed /= REVERSE_SPEED_OFFSET;
    }

    dir.x *= (double)newSpeed * ACCELERATION_FACTOR;
    dir.y *= (double)newSpeed * ACCELERATION_FACTOR;
    setHoverSetpoint(setpoint, dir.x, dir.y, altitude, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    return;
}

CFVector GetPosition(void* p) {
    logVarId_t logIdPosX = logGetVarId("stateEstimate", "x");
    logVarId_t logIdPosY = logGetVarId("stateEstimate", "y");
    logVarId_t logIdPosZ = logGetVarId("stateEstimate", "z");
    CFVector pos = {0, 0, 0};
    pos.x = logGetFloat(logIdPosX);
    pos.y = logGetFloat(logIdPosY);
    pos.z = logGetFloat(logIdPosZ);
    return pos;
}

float GetAngle(void* p, bool withOffset) {
    logVarId_t paramIdStabilizerController = logGetVarId("stabilizer", "yaw");
    return logGetFloat(paramIdStabilizerController) * ((float) (M_PI / HALF_CIRCLE_DEGREES));
}

bool Wait(int* counter, int delay, void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    setHoverSetpoint(setpoint, 0, 0, altitude, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    int max_iter = TICKS_PER_SECOND * delay;

    if ((*counter) < max_iter) {
        (*counter)++;
        return false;
    }

    (*counter) = 0;
    return true;
}

void Rotate(double radianAngle, void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    yaw = radianAngle * (HALF_CIRCLE_DEGREES / M_PI);
    setHoverSetpoint(setpoint, 0, 0, altitude, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    return;
}

void Stop(void* p) {
    setpoint_t* setpoint = (setpoint_t*)p;
    setHoverSetpoint(setpoint, 0, 0, altitude, yaw);
    commanderSetSetpoint(setpoint, SETPOINT_PRIORITY);
    return;
}

CFVector GetLeftPosition(CFReturnToBase params, void* p) {
    float orientation = GetAngle(p, true);
    CFVector dir = {0, 0, 0};
    dir.x += cos(orientation);
    dir.y += sin(orientation);
    dir.z = GetDistanceFromGround(p);
    return dir;
}

/* Not needed for drones */
void Print(char* str) {
    return;
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
        &Print,
    };

    return commander;
}
