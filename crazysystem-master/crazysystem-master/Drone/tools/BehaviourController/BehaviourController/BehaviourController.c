#include "BehaviourController.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

bool TakeOff(const float height, Commander *commander, void *p) {
    (*commander).GoUp(height, p);
    return (height - (*commander).GetDistanceFromGround(p) < HEIGHT_DIFF);
}

bool Land(CFVector pos, Commander *commander, void *p) {
    (*commander).GoToGround(pos, p);
    return ((*commander).GetDistanceFromGround(p) < HEIGHT_LAND);
}

CFDroneExplore Explore(CFDroneExplore droneParams, Commander *commander, void *p) {
    CFVector position = (*commander).GetPosition(p);
    int randomSeed = (int)((position.x + position.y + position.z) * 1000);
    srand(randomSeed);
    int randomDirection = -1;
    double* wallPositions = (*commander).GetDistanceFromWalls(p, droneParams.wallPositions);

    switch (droneParams.exploration_state) {

        case GET_RANDOM_POINT: {
            while(randomDirection == -1) {
                randomDirection = (enum CFDirection)rand() % NUMBER_OF_DIRECTIONS;
                if(
                    wallPositions[randomDirection] < (PERCENT_150 * WALL_DISTANCE)
                    && wallPositions[randomDirection] > 0
                ) {
                    randomDirection = -1;
                }
            }
            droneParams.direction = randomDirection;
            droneParams.exploration_state = EXPLORE;
            break;
        }

        case EXPLORE: {
            MoveInDirection(BASE_SPEED, droneParams.direction, commander, p);
            for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++) {
                if (wallPositions[i] < WALL_DISTANCE && wallPositions[i] >= 0) {
                    droneParams.direction = i;
                    MoveInDirection(REVERSE_SPEED, droneParams.direction, commander, p);
                    droneParams.exploration_state = MOVE_AWAY;
                }
            }
            break;
        }

        case MOVE_AWAY: {
            MoveInDirection(REVERSE_SPEED, droneParams.direction, commander, p);
            for(int i = 0; i < NUMBER_OF_DIRECTIONS; i++) {
                if (
                    wallPositions[i] < WALL_DISTANCE &&
                    wallPositions[i] >= 0 &&
                    wallPositions[i] < wallPositions[droneParams.direction]
                ) {
                    droneParams.direction = i;
                    MoveInDirection(REVERSE_SPEED, droneParams.direction, commander, p);
                }
            }
            if (wallPositions[droneParams.direction] > WALL_DISTANCE || wallPositions[droneParams.direction] < 0) {
                droneParams.exploration_state = ROTATE;
                double angle = (double)((*commander).GetAngle(p, false)) + (M_PI / 16);
                (*commander).Rotate(angle, p);
            }
            break;
        }

        case ROTATE: {
            if ((*commander).Wait(&(droneParams.counter), 2, p)) {
                droneParams.exploration_state = GET_RANDOM_POINT;
            }
            break;
        }
    }

    return droneParams;
}

bool ReturnToBase(CFReturnToBase* droneParams, Commander *commander, void *p) {
    CFVector dronePosition = (*commander).GetPosition(p);
    double *walls = (*commander).GetDistanceFromWalls(p, (*droneParams).wallPositions);
    CFVector zeroVector = {0, 0, 0};

    if (
        fabs(dronePosition.x - (*droneParams).basePosition.x) < BASE_OFFSET && 
        fabs(dronePosition.y - (*droneParams).basePosition.y) < BASE_OFFSET
    ) {
        (*droneParams).currentState = ARRIVED;
        return true;
    }

    switch((*droneParams).currentState) {

        case ROTATING: {
            (*droneParams).currentSensor =
            (GetQuadrant((*droneParams).basePosition, commander, p)
            + (*commander).quadrantOffset) % NUMBER_OF_QUADRANTS;
            (*droneParams).desiredPosition = (*commander).GetPosition(p);
            RotateInDirection((*droneParams).basePosition, commander, p);
            (*droneParams).currentState = WAITING;
            break;
        }

        case WAITING: {
            (*commander).SetDestination((*droneParams).desiredPosition, p, zeroVector);
            if ((*commander).Wait(&(droneParams->counter), SECONDS_5, p)) {
                (*droneParams).currentState = MOVING;
            }
            break;
        }

        case MOVING: {
            CFVector unitVector = GetUnitVectorToPoint(
                (*droneParams).basePosition,
                commander,
                walls[(*droneParams).currentSensor],
                p
            );
            CFVector goToVector = {dronePosition.x + unitVector.x, dronePosition.y + unitVector.y, unitVector.z};
            CFVector dir = GetDirection((*droneParams).currentSensor, commander);
            (*commander).SetDestination(goToVector, p, dir);
            if (walls[(*droneParams).currentSensor] < WALL_DISTANCE && walls[(*droneParams).currentSensor] > 0) {
                (*droneParams).currentState = CHECKING_WALLS;
                (*droneParams).counter = 0;
                CFVector position = {0, 0, (*commander).GetDistanceFromGround(p)};
                position.x = dronePosition.x - unitVector.x * 2;
                position.y = dronePosition.y - unitVector.y * 2;
                (*droneParams).desiredPosition = position;
            }
            break;
        }

        case CHECKING_WALLS: {
            CFVector reverseDir = GetDirection((((*droneParams).currentSensor + 2) % NUMBER_OF_DIRECTIONS), commander);
            (*commander).SetDestination((*droneParams).desiredPosition, p, reverseDir);
            if(walls[(*droneParams).currentSensor] > WALL_DISTANCE || walls[(*droneParams).currentSensor] < 0) {
                (*droneParams).currentState = DODGING;
                (*droneParams).desiredPosition = (*commander).GetPosition(p);
                MoveInDirection(0.0f, (*droneParams).currentSensor, commander, p);
            }
            break;
        }

        case DODGING: {
            int sensorLeft = (((*droneParams).currentSensor + 1) % NUMBER_OF_QUADRANTS);
            if(walls[sensorLeft] < WALL_DISTANCE && walls[sensorLeft] > 0) {
                (*droneParams).currentState = ROTATING;
                break;
            }
            MoveInDirection(BASE_SPEED, sensorLeft, commander, p);
            if(walls[(*droneParams).currentSensor] < WALL_DISTANCE && walls[(*droneParams).currentSensor] > 0) {
                MoveInDirection(ESCAPE_SPEED, (*droneParams).currentSensor, commander, p);
            }
            if(walls[(*droneParams).currentSensor] > (WALL_DISTANCE * PERCENT_150) || walls[(*droneParams).currentSensor] < 0) {
                (*droneParams).counter++;
                if((*droneParams).counter > ((*commander).ticksPerSecond * 2)) {
                    (*droneParams).desiredPosition = (*commander).GetPosition(p);
                    (*droneParams).currentState = ROTATING;
                    (*droneParams).counter = 0;
                }
            }
            break;
        }

        case ARRIVED: {
            (*commander).SetDestination((*droneParams).basePosition, p, zeroVector);
            if ((*commander).Wait(&(droneParams->counter), SECONDS_3, p)) {
                return true;
            }
            break;
        }
    }

    return false;
}

void MoveInDirection(const float speed, enum CFDirection direction, Commander *commander, void* p) {
    CFVector dir = GetDirection(direction, commander);
    if (fabs(dir.x) < SMALL_DIRECTION) {
        dir.x = 0;
    }
    if (fabs(dir.y) < SMALL_DIRECTION) {
        dir.y = 0;
    }
    (*commander).SetLinearVelocity(speed, dir, p);
}

CFVector GetDirection(enum CFDirection direction, Commander *commander) {
    float orientation = (*commander).angleOffset;
    CFVector dir = {0, 0, 0};

    switch (direction) {

        case FORWARD:
            dir.x += cos(orientation);
            dir.y += sin(orientation);
            break;

        case LEFT:
            dir.x -= sin(orientation);
            dir.y += cos(orientation);
            break;

        case BACKWARD:
            dir.x -= cos(orientation);
            dir.y -= sin(orientation);
            break;

        case RIGHT:
            dir.x += sin(orientation);
            dir.y -= cos(orientation);
            break;
    }

    return dir;
}

int GetQuadrant(CFVector pos, Commander *commander, void* p) {
    CFVector dronePosition = (*commander).GetPosition(p);
    if(pos.x > dronePosition.x && pos.y > dronePosition.y) return 0;
    if(pos.x < dronePosition.x && pos.y > dronePosition.y) return 1;
    if(pos.x < dronePosition.x && pos.y < dronePosition.y) return 2;
    if(pos.x > dronePosition.x && pos.y < dronePosition.y) return 3;
    return -1;
}

void RotateInDirection(CFVector pos, Commander *commander, void* p) {
    CFVector dronePosition = (*commander).GetPosition(p);
    double opposed = pos.y - dronePosition.y;
    double adjacent = pos.x - dronePosition.x;
    double angle = 0;
    double rad = 0;

    if(fabs(adjacent) == 0) {
        angle = M_PI / 2;
    } else {
        angle = atan(fabs(opposed) / fabs(adjacent));
    }

    if((opposed < 0 && adjacent < 0) || (opposed > 0 && adjacent > 0)) {
        rad = angle;
    } else {
        rad = M_PI / 2 - angle;
    }
    
    (*commander).Rotate(rad, p);
}

CFVector GetUnitVectorToPoint(CFVector basePosition, Commander *commander, double wallDistance, void* p) {
    CFVector dronePosition = (*commander).GetPosition(p);
    double xDistance = basePosition.x - dronePosition.x;
    double yDistance = basePosition.y - dronePosition.y;
    double distance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));
    
    CFVector unitVector = {
        xDistance / (distance * NUMBER_OF_DIRECTIONS),
        yDistance / (distance * NUMBER_OF_DIRECTIONS),
        (*commander).GetDistanceFromGround(p)
    };

    if(distance < 1) {
        unitVector.x = xDistance / UNIT_VECTOR_DIVIDER;
        unitVector.y = yDistance / UNIT_VECTOR_DIVIDER;
    }

    if(wallDistance < METER_IN_CM && wallDistance > 0) {
        unitVector.x = unitVector.x * (wallDistance / METER_IN_CM);
        unitVector.y = unitVector.y * (wallDistance / METER_IN_CM);
    }

    return unitVector;
}
