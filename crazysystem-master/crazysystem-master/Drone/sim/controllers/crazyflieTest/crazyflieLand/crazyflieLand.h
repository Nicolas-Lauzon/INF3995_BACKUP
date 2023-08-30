#ifndef CRAZYFLIE_LAND_H
#define CRAZYFLIE_LAND_H
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


/*
 * A controller is simply an implementation of the CCI_Controller class.
 */
class CCrazyflieLand : public CCI_Controller {
    public:
    /* Class constructor. */
    CCrazyflieLand();
    /* Class destructor. */
    virtual ~CCrazyflieLand() {}

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

    /* Object containing the possible commands of the drones */
    Commander commander;

    /* Structure of the sensors */
    SSensors *sensors;

};

#endif
