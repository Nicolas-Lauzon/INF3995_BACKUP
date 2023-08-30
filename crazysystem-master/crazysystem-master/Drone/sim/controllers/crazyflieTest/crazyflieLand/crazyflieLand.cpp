/* Include the controller definition */
#include "crazyflieLand.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cmath>


using namespace std;

/****************************************/
/****************************************/

CCrazyflieLand::CCrazyflieLand()
    : m_pcDistance(NULL),
      m_pcPropellers(NULL),
      m_pcPos(NULL),
      m_pcP2PSend(NULL),
      m_pcP2PReceive(NULL),
      m_pcBattery(NULL) {}

/****************************************/
/****************************************/

void CCrazyflieLand::Init(TConfigurationNode &t_node) {
    try {
        /*
         * Initialize sensors/actuators
         */
        m_pcDistance = GetSensor<CCI_CrazyflieDistanceScannerSensor>(
            "crazyflie_distance_scanner");
        m_pcPropellers =
            GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
        /* Get pointers to devices */
        m_pcP2PSend =
            GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
        m_pcP2PReceive =
            GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");
        try {
            m_pcPos = GetSensor<CCI_PositioningSensor>("positioning");
        } catch (CARGoSException &ex) {
        }
        try {
            m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
        } catch (CARGoSException &ex) {
            LOG << ex.what() << std::endl;
        }
    } catch (CARGoSException &ex) {
        THROW_ARGOSEXCEPTION_NESTED(
            "Error initializing the crazyflie path controller for robot \""
                << GetId() << "\"",
            ex);
    }
    commander = SetCommander();
    sensors = (SSensors *)calloc(1, sizeof(SSensors));
    sensors->m_pcBattery = m_pcBattery;
    sensors->m_pcPos = m_pcPos;
    sensors->m_pcDistance = m_pcDistance;
    sensors->m_pcPropellers = m_pcPropellers;
    sensors->m_pcP2PSend = m_pcP2PSend;
    sensors->m_pcP2PReceive = m_pcP2PReceive;
}

/****************************************/
/****************************************/

void CCrazyflieLand::ControlStep() {
    void *pointer = static_cast<void*>(sensors);
    CFVector landingPosition = { GetPosition(pointer).x, GetPosition(pointer).y, 0 };
    if(Land(landingPosition, &commander, pointer)) {
        LOG << "Landing done\n";
    }
}

/****************************************/
/****************************************/

void CCrazyflieLand::Reset() {}

/****************************************/
/****************************************/


/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the XML configuration file to refer to
 * this controller.
 * When ARGoS reads that string in the XML file, it knows which controller
 * class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CCrazyflieLand, "crazyflie_land")
