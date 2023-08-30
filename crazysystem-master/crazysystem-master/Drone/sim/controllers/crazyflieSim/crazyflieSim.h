#ifndef CRAZYFLIE_SIM_H
#define CRAZYFLIE_SIM_H
/*
 * Include some necessary headers.
 */
/* Definition of the CCI_Controller class. */
#include <argos3/core/control_interface/ci_controller.h>
/* Definition of the crazyflie distance sensor */
#include <argos3/plugins/robots/crazyflie/control_interface/ci_crazyflie_distance_scanner_sensor.h>
/* Definition of the crazyflie position actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_quadrotor_position_actuator.h>
/* Definition of the crazyflie position sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
/* Definition of the crazyflie range and bearing actuator */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
/* Definition of the crazyflie range and bearing sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
/* Definition of the crazyflie battery sensor */
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
/* Definitions for random number generation */
#include <argos3/core/utility/math/rng.h>
/* Common library between crazyflie and ARGoS simulation */

#include <argos3/core/utility/networking/tcp_socket.h>

extern "C" {
#include "/root/sim/tools/BehaviourController/BehaviourController/BehaviourController.h"
}
/* Library specific to ARGoS simulation */
#include "/root/sim/tools/BehaviourController/BehaviourControllerSimulation/BehaviourControllerSimulation.h"

/*
 * All the ARGoS stuff in the 'argos' namespace.
 * With this statement, you save typing argos:: every time.
 */
using namespace argos;
using namespace std;

#define PORT 5200
#define BUFFER_SIZE 1024
#define SOCKET_ADDRESS "127.0.0.1"
#define BATTERY_30 0.3f
#define DRONE_HEIGHT 0.5
#define DRONE_DISTANCE 50
#define METER_TO_CM 100
#define METER_TO_MM 1000
#define CM_TO_MM 10
#define NUMBER_OF_QUADRANTS 4
#define TICKS_PER_SECOND 20
#define MAX_POSITION 15000
#define SOCKET_ID 0
#define SOCKET_STATE 1
#define SOCKET_SPEED 2
#define SOCKET_BATTERY 3
#define SOCKET_POS_X 4
#define SOCKET_POS_Y 5
#define SOCKET_POS_Z 6
#define SOCKET_WALL_FRONT 7
#define SOCKET_WALL_LEFT 8
#define SOCKET_WALL_BACK 9
#define SOCKET_WALL_RIGHT 10
#define SOCKET_ANGLE 11

typedef struct SimP2PPacket {
    float x;
    float y;
    uint8_t id;
} CFP2PPacket;

enum DroneStates {
    STANDBY = 0,
    IN_MISSION = 1,
    CRASHED = 2,
    TAKEOFF = 3,
    LANDING = 4,
    RETURN_TO_BASE = 5
};

/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CCrazyflieSim : public CCI_Controller {
    public:
    /* Class constructor. */
    CCrazyflieSim();
    /* Class destructor. */
    virtual ~CCrazyflieSim() {}

    /*
     * This function initializes the controller.
     * The 't_node' variable points to the <parameters> section in the XML
     * file in the <controllers><crazyflie_path_controller> section.
     */
    virtual void Init(TConfigurationNode &t_node);

    /*
     * This function is called once every time step.
     * The length of the time step is set in the XML file.
     */
    virtual void ControlStep();

    /*
     * This function resets the controller to its state right after the
     * Init().
     * It is called when you press the reset button in the GUI.
     */
    virtual void Reset();

    /*
     * Called to cleanup what done by Init() when the experiment finishes.
     * In this example controller there is no need for clean anything up,
     * so the function could have been omitted. It's here just for
     * completeness.
     */
    virtual void Destroy() {}

    /*
     * This function waits for the instructions from the station
     */
    void WaitForInstructions();

    void socketReceive();

    private:
    /* Pointer to the crazyflie distance sensor */
    CCI_CrazyflieDistanceScannerSensor *m_pcDistance;

    /* Pointer to the position actuator */
    CCI_QuadRotorPositionActuator *m_pcPropellers;

    /* Pointer to the positioning sensor */
    CCI_PositioningSensor *m_pcPos;

    /* Pointer to the battery sensor */
    CCI_BatterySensor *m_pcBattery;

    /* Pointer to the range and bearing actuator */
    CCI_RangeAndBearingActuator *m_pcP2PSend;

    /* Pointer to the range and bearing sensor */
    CCI_RangeAndBearingSensor *m_pcP2PReceive;

    CTCPSocket tcpSocket;

    /* Object containing the possible commands of the drones */
    Commander commander;

    /* Structure of the sensors */
    SSensors *sensors;

    /* Enum of the current robot state */
    enum EState { STATE_TAKE_OFF, STATE_EXPLORE, STATE_LANDING, STATE_WAITING_ON_GROUND, STATE_RETURN_TO_BASE };

    /* Current robot state */
    EState m_eState;

    int connectedSocket;

    bool firstCallToLand;

    CFVector positionToLand;

    CFDroneExplore droneExplore;

    CFReturnToBase droneReturnToBase;

    int socketInfo[12];

    int sendCounter;

    bool batteryLow;
};

#endif
