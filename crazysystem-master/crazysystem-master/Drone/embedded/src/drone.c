#include "app.h"
#include "param.h"
#include "log.h"
#include "config.h"
#include "radiolink.h"
#include "FreeRTOS.h"
#include "task.h"
#include "configblock.h"
#include "led.h"
#include "debug.h"
#include "sitaw.h"
#include "commander.h"
#include "/root/embedded/tools/BehaviourController/BehaviourController/BehaviourController.h"
#include "/root/embedded/tools/BehaviourController/BehaviourControllerDrone/BehaviourControllerDrone.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define DEBUG_MODULE "LEDCONTROL"
#define MIN_UP_DISTANCE 300
#define CRAZYFLIE_BASE_ADDRESS 996028180224
#define MS_8000 8000
#define CHANNEL_60 60
#define MS_2500 2500
#define MS_6500 6500
#define MS_250 250
#define MAX_VBAT 4.2f
#define LOW_BAT_FLIGHT 3.25f
#define LOW_BAT_STATIC 3.77f
#define BASE_HEIGHT 0.0f
#define HEIGHT_DIFFERENCE 0.35f
#define LOWEST_HEIGHT 0.05f
#define HEIGHT_OFFSET 0.02f
#define METER_IN_MM 1000.0f

enum State {
    STATE_STANDBY = 0,
    STATE_IN_MISSION,
    STATE_CRASHED,
    STATE_TAKE_OFF,
    STATE_LANDING,
    STATE_RETURN_TO_BASE
};

enum Exploration {
    WAITING_ON_GROUND,
    TAKE_OFF,
    LAND,
    RETURN_TO_BASE,
    EXPLORE_ROOM,
    CRASHED,
};

static enum State state = STATE_STANDBY;
static enum Exploration exploreState = WAITING_ON_GROUND;

uint64_t baseId = CRAZYFLIE_BASE_ADDRESS, droneId = 0;

uint32_t currentTick = 0;

uint8_t P2PHeighestNumber = 0, droneNumber = 0, takeOff = 0, land = 0, returnToBase = 0;

int countP2PCalls = -1;
int mutexP2P = 1, mutexHeighestNumber = 1;

bool batteryLow = false;

CFDroneExplore droneExplore = {EXPLORE, FORWARD, 0, NULL};
CFReturnToBase droneReturnToBase = {ROTATING, ROTATING, 0, 0, {0, 0, 0}, {0, 0, 0}, NULL};

void p2pcallbackHandler(P2PPacket *p) {
    if (mutexP2P == 1) {
        countP2PCalls++;
    }
    uint8_t value = p->data[1];

    if (mutexHeighestNumber == 1) {
        if (value > P2PHeighestNumber) {
            P2PHeighestNumber = value;
        }
    }
}

void setupP2PWithIds() {
    p2pRegisterCB(p2pcallbackHandler);
    radiolinkSetChannel(CHANNEL_60);

    uint32_t previousTick = currentTick;
    while (true) {
        currentTick = xTaskGetTickCount();
        uint32_t delta = currentTick - previousTick;
        if (delta >= M2T(MS_8000)) { // M2T is millisecondsToTicks, so it converts 8000 ms to ticks
            break;
        }
    }
    mutexP2P--;
    if (countP2PCalls == -1) {
        P2PHeighestNumber = 1;
        droneNumber = P2PHeighestNumber;
        droneId = baseId + droneNumber;
        ledSetAll();
        vTaskDelay(M2T(MS_2500));
        ledClearAll();
    } else {
        droneNumber = P2PHeighestNumber + 1;
        droneId = baseId + droneNumber;
        ledSetAll();
        vTaskDelay(M2T(MS_6500));
        ledClearAll();
    }
    mutexHeighestNumber--;
    radiolinkSetAddress(droneId);
}

P2PPacket P2PSendInfo() {
    static P2PPacket p_reply;
    p_reply.port = 0x00;
    uint64_t address = configblockGetRadioAddress();
    uint8_t my_id = (uint8_t)((address)&0x00000000ff);
    p_reply.data[0] = my_id;
    p_reply.data[1] = droneNumber;
    p_reply.size = 2;
    return p_reply;
}

void appMain() {
    /* declares and initializes variables for battery voltage and top ranging deck */
    logVarId_t vbatId = logGetVarId("pm", "vbat");
    float vbat = logGetFloat(vbatId);

    logVarId_t idRangingDeckUp = logGetVarId("range", "up");
    uint16_t rangingDeckUp = logGetUint(idRangingDeckUp);

    float lastVbatReadings[3] = {MAX_VBAT, MAX_VBAT, MAX_VBAT};

    setupP2PWithIds();

    P2PPacket p_reply = P2PSendInfo();

    /* setup for the exploreState switch case */
    static setpoint_t setpoint;
    CFVector zero = {0, 0, 0};
    Commander commander = SetCommander();
    void* pointer = (void*)(&setpoint);

    while (true) {
        vTaskDelay(M2T(MS_250));
        
        /* send a P2P packet in broadcast mode. The packet just has the drone's ID (count) */
        radiolinkSendP2PPacketBroadcast(&p_reply);

        /* gets the necessary info each 250 ms */
        vbat = logGetFloat(vbatId);
        rangingDeckUp = logGetUint(idRangingDeckUp);

        /* push new vbat and pop 3rd value */
        lastVbatReadings[2] = lastVbatReadings[1];
        lastVbatReadings[1] = lastVbatReadings[0];
        lastVbatReadings[0] = vbat;

        /* if the drone has crashed, then go to state crashed */
        if(sitAwTuDetected()) {
            exploreState = CRASHED;
        }

        /* if either land or return to base have been received (commands), then set the other values to 0
        and go the desired state. */
        if (land == 1) {
            takeOff = 0;
            returnToBase = 0;
            exploreState = LAND;
        }
        if (
            returnToBase == 1 ||
            (
                lastVbatReadings[0] < LOW_BAT_FLIGHT &&
                lastVbatReadings[1] < LOW_BAT_FLIGHT &&
                lastVbatReadings[2] < LOW_BAT_FLIGHT
            )
        ) {
            takeOff = 0;
            land = 0;
            exploreState = RETURN_TO_BASE;
        }

        /* This code here is for the drones dynamic height (and crash if they're in a small hole for example) */
        float baseHeight = BASE_HEIGHT + droneNumber * HEIGHT_DIFFERENCE;
        float droneHeight = commander.GetDistanceFromGround(NULL);
        if(rangingDeckUp < MIN_UP_DISTANCE) {
            baseHeight = droneHeight - ((MIN_UP_DISTANCE - rangingDeckUp) / METER_IN_MM) - HEIGHT_OFFSET;
            if(baseHeight > LOWEST_HEIGHT) {
                commander.GoUp(baseHeight, pointer);
                state = STATE_IN_MISSION;
            } else {
                exploreState = CRASHED;
            }
        } else if(droneHeight < baseHeight) {
            commander.GoUp(baseHeight, pointer);
        }
        
        /* The main switch case for the drone to explore and do other states */
        switch(exploreState) {

            case TAKE_OFF:
                if(!batteryLow) {
                    state = STATE_TAKE_OFF;
                    if(TakeOff(baseHeight, &commander, pointer)) {
                        exploreState = EXPLORE_ROOM;
                        takeOff = 0;
                    }
                } else {
                    exploreState = WAITING_ON_GROUND;
                    takeOff = 0;
                }
                break;

            case EXPLORE_ROOM:
                state = STATE_IN_MISSION;
                droneExplore = Explore(droneExplore, &commander, pointer);
                break;

            case RETURN_TO_BASE:
                returnToBase = 0;
                state = STATE_RETURN_TO_BASE;
                if(ReturnToBase(&droneReturnToBase, &commander, pointer)) {
                    exploreState = LAND;
                }
                break;

            case LAND:
                state = STATE_LANDING;
                if(Land(zero, &commander, pointer)) {
                    exploreState = WAITING_ON_GROUND;
                    land = 0;
                }
                break;

            case CRASHED:
                state = STATE_CRASHED;
                memset(&setpoint, 0, sizeof(setpoint_t));
                commanderSetSetpoint(&setpoint, 3);
                break;

            case WAITING_ON_GROUND:
            default:
                state = STATE_STANDBY;
                memset(&setpoint, 0, sizeof(setpoint_t));
                commanderSetSetpoint(&setpoint, 3);
                if(takeOff == 1 && vbat > LOW_BAT_STATIC) {
                    land = 0;
                    returnToBase = 0;
                    exploreState = TAKE_OFF;
                }
                break;
        }
    }
}
PARAM_GROUP_START(commands)
    PARAM_ADD(PARAM_UINT8, takeOff, &takeOff)
    PARAM_ADD(PARAM_UINT8, land, &land)
    PARAM_ADD(PARAM_UINT8, returnToBase, &returnToBase)
PARAM_GROUP_STOP(commands)

LOG_GROUP_START(droneInfo)
    LOG_ADD(LOG_UINT8, state, &state)
    LOG_ADD(LOG_UINT8, droneNumber, &droneNumber)
LOG_GROUP_STOP(droneInfo)
