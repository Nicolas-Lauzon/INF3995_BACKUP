#ifndef BEHAVIOUR_CONTROLLER_H
#define BEHAVIOUR_CONTROLLER_H

#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define WALL_DISTANCE 30
#define HEIGHT_DIFF 0.05f
#define HEIGHT_LAND 0.1f
#define NUMBER_OF_DIRECTIONS 4
#define NUMBER_OF_QUADRANTS 4
#define PERCENT_150 1.5
#define BASE_SPEED 0.05f
#define REVERSE_SPEED -0.3f
#define ESCAPE_SPEED -0.1f
#define BASE_OFFSET 0.15
#define SECONDS_5 5
#define SECONDS_3 5
#define SMALL_DIRECTION 0.001
#define METER_IN_CM 100
#define UNIT_VECTOR_DIVIDER 6

/**
 * Struct representing a crazyflie movement vector
 */
typedef struct CFVector {
    double x;
    double y;
    double z;
} CFVector;

/**
 * Struct representing the ranging deck values
 */
typedef struct CFRangingDeck {
    double front;
    double left;
    double back;
    double right;
} CFRangingDeck;

/**
 * Enum for the different directions that the drones can take
 */
enum CFDirection {
    FORWARD = 0,
    LEFT = 1,
    BACKWARD = 2,
    RIGHT = 3,
};

/**
 * Enum for the different states for the return to base functionality
 */
enum CFReturnToBaseStates { ROTATING, MOVING, CHECKING_WALLS, WAITING, DODGING, ARRIVED };

/**
 * Struct representing the return to base functionality. It's used for the
 * algorithm for the drones to return to base
 */
typedef struct CFReturnToBase {
    enum CFReturnToBaseStates currentState;
    enum CFReturnToBaseStates nextState;
    int counter;
    int currentSensor;
    CFVector desiredPosition;
    CFVector basePosition;
    double* wallPositions;
} CFReturnToBase;

/**
 * Enum for the different exploration states that the drones will do
 * while exploring
 */
enum ExplorationStates {
    GET_RANDOM_POINT,
    EXPLORE,
    MOVE_AWAY,
    ROTATE,
};

/**
  * Struct representing the elements needed for the drones to explore
  */
typedef struct CFDroneExplore {
    enum ExplorationStates exploration_state;
    enum CFDirection direction;
    int counter;
    double* wallPositions;
} CFDroneExplore;

typedef struct Commander {

    float angleOffset;
    int quadrantOffset;
    int ticksPerSecond;

    /**
     * Makes the drone go up.
     * @param height set the desired height
     * @param p generic parameter that we cast to the needed type
     */
    void (*GoUp)(const float height, void* p);

    /**
     * Makes the drone go to the ground.
     * @param pos desired (actual) position for the drone
     * @param p generic parameter that we cast to the needed type
     */
    void (*GoToGround)(CFVector pos, void* p);

    /**
     * Gets the distance between the drone and the ground.
     * @param p generic parameter that we cast to the needed type
     * @return distance from the ground
     */
    float (*GetDistanceFromGround)(void* p);

    /**
     * Gets the distance from walls in this order: front, left, back, right
     * @param p generic parameter that we cast to the needed type
     * @return distance from walls
     */
    double* (*GetDistanceFromWalls)(void* p, double* wallPositions);

    /**
     * Sets the desired destination
     * @param dest sets the time that it should take to get to destination
     * @param p generic parameter that we cast to the needed type
     * @return distance to destination 
     */
    float (*SetDestination)(const CFVector dest, void* p, CFVector direction);

    /**
     * Sets the drone's speed
     * @param speed the desired speed
     * @param dir the desired direction
     * @param p generic parameter that we cast to the needed type
     */
    void (*SetLinearVelocity)(const float speed, CFVector dir, void* p);

    /**
     * Gets the current position of the drone
     * @param p generic parameter that we cast to the needed type
     * @return CFVector of the drone
     */
    CFVector (*GetPosition)(void* p);

    /**
     * Gets the drone's current angle
     * @param p generic parameter that we cast to the needed type
     * @param withOffset tells us if the angle has an offset or not
     * @return the angle
     */
    float (*GetAngle)(void* p, bool withOffset);

    /**
     * Makes the drone wait
     * @param counter the counter to wait
     * @param delay the delay to wait
     * @param p generic parameter that we cast to the needed type
     * @returns true if the wait is finished, false otherwise
     */
    bool (*Wait)(int* counter, int delay, void* p);

    /**
     * Makes the drone rotate
     * @param radianAngle the desired angle in radian
     * @param p generic parameter that we cast to the needed type
     */
    void (*Rotate)(double radianAngle, void* p);

    /**
     * Makes the drone stop everything (but not fall)
     * @param p generic parameter that we cast to the needed type
     */
    void (*Stop)(void* p);

    /**
     * Gets the left position of the drone. This is used for the return to base
     * functionality
     * @param params struct for return to base
     * @param p generic parameter that we cast to the needed type
     * @return CFVector of the left position
     */
    CFVector (*GetLeftPosition)(CFReturnToBase params, void* p);

    /**
     * Allows us to print info to debug
     * @param c the char we need to print
     */
    void (*Print)(char* c);
} Commander;

/* These are the core functions that will be called from the controllers */

/** 
 * Makes the robot take off from the ground
 * @param height The desired height
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors. Set to null for physical robots
 * @return returns whether the movement is completed or not
 */
bool TakeOff(const float height, Commander *commander, void *p);

/** 
 * Makes the robot land where he started
 * @param droneParams Contains information needed by the robot
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors. Must be set to null for physical robots
 */
bool ReturnToBase(CFReturnToBase* droneParams, Commander *commander, void *p);

/** 
 * Makes the robot land at his current x-y position. This is considered the emergency landing
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors. Must be set to null for physical robots
 */
bool Land(CFVector pos, Commander *commander, void *p);

/** 
 * Makes the robot explore a room for a given duration
 * @param droneParams The exploration parameters of the drone
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors. Must be set to null for physical robots
 */
CFDroneExplore Explore(CFDroneExplore droneParams, Commander *commander, void *p);

/** 
 * Makes the robot move in wanted cardinal direction
 * @param speed Relative speed for drone
 * @param direction The wanted direction (Left, right, forward, backward)
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors, or physical setpoints
 */
void MoveInDirection(const float speed, enum CFDirection direction, Commander *commander, void* p);

/** 
 * Transform the CFDirection into a unit vector
 * @param direction The wanted direction (Left, right, forward, backward)
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 */
CFVector GetDirection(enum CFDirection direction, Commander *commander);

/** 
 * Get the quadrant of position, relative to robot
 * @param pos Position of base station
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors, or physical setpoints
 */
int GetQuadrant(CFVector pos, Commander *commander, void* p);

/** 
 * Rotate drone in direction of wanted point
 * @param pos Position of point to ratate towards
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param p The struct containing the simulation sensors, or physical setpoints
 */
void RotateInDirection(CFVector pos, Commander *commander, void* p);

/** 
 * Gets a unit vector from drone position towards base position
 * @param basePosition Position of base
 * @param commander The commander containing the movement functions of the robot (either simulation or crazyflie)
 * @param wallDistance The distance from the wall
 * @param p The struct containing the simulation sensors, or physical setpoints
 * @return the unit vector rom drone position towards base position
 */
CFVector GetUnitVectorToPoint(CFVector basePosition, Commander *commander, double wallDistance, void* p);

#endif
