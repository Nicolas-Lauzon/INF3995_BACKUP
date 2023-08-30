/* Include the controller definition */
#include "crazyflieSim.h"
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
#include <map>
#include <cmath>

using namespace std;

std::map<string, int> DRONE_IDS {{"drone10", 1}, {"drone11", 2}, {"drone12", 3}, {"drone13", 4}};

/****************************************/
/****************************************/

CCrazyflieSim::CCrazyflieSim()
    : m_pcDistance(NULL),
      m_pcPropellers(NULL),
      m_pcPos(NULL),
      m_pcP2PSend(NULL),
      m_pcP2PReceive(NULL),
      m_pcBattery(NULL),
      m_eState(STATE_WAITING_ON_GROUND),
      batteryLow(false) {}

/****************************************/
/****************************************/

void CCrazyflieSim::Init(TConfigurationNode &t_node) {
    try {
        /* Initialize sensors/actuators */
        m_pcDistance = GetSensor<CCI_CrazyflieDistanceScannerSensor>("crazyflie_distance_scanner");
        m_pcPropellers = GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
        /* Get pointers to devices */
        m_pcP2PSend = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
        m_pcP2PReceive = GetSensor<CCI_RangeAndBearingSensor>("range_and_bearing");

        try {
            m_pcPos = GetSensor<CCI_PositioningSensor>("positioning");
        } catch (CARGoSException &ex) {
            LOG << ex.what() << std::endl;
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
    sensors = (SSensors*)calloc(1, sizeof(SSensors));
    sensors->m_pcBattery = m_pcBattery;
    sensors->m_pcPos = m_pcPos;
    sensors->m_pcDistance = m_pcDistance;
    sensors->m_pcPropellers = m_pcPropellers;
    sensors->m_pcP2PSend = m_pcP2PSend;
    sensors->m_pcP2PReceive = m_pcP2PReceive;
    
    firstCallToLand = false;
    connectedSocket = 0;
    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE] = {0};
    if ((connectedSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {}
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SOCKET_ADDRESS, &serv_addr.sin_addr)<=0) {}
    if(connect(connectedSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {}
    
    double* wallPositions = new double;
    droneExplore = {EXPLORE, FORWARD, 0, wallPositions};

    if (GetId() == "drone10") droneExplore.direction = FORWARD;
    else if (GetId() == "drone11") droneExplore.direction = LEFT;
    else if (GetId() == "drone12") droneExplore.direction = BACKWARD;
    else if (GetId() == "drone13") droneExplore.direction = RIGHT;

    CFVector basePosition = {
        m_pcPos->GetReading().Position.GetX(),
        m_pcPos->GetReading().Position.GetY(),
        m_pcPos->GetReading().Position.GetZ()
    };
    double* wallPositionsReturn = new double;
    droneReturnToBase = {ROTATING, ROTATING, 0, 0, {0, 0, 0}, basePosition, wallPositionsReturn};

    socketInfo[SOCKET_STATE] = STANDBY;

    Reset();
}

/****************************************/
/****************************************/

void CCrazyflieSim::ControlStep() {
    void *pointer = static_cast<void *>(sensors);
    
    try {
        socketReceive();
    } catch (CARGoSException &ex) {
        LOG << ex.what() << std::endl;
    }

    uint8_t id = DRONE_IDS[GetId()];

    /* Send position by P2P */
    float xPosition = (float)m_pcPos->GetReading().Position.GetX();
    float yPosition = (float)m_pcPos->GetReading().Position.GetY();
    const SimP2PPacket sendPacket = {xPosition, yPosition, id};
    CByteArray cbuf(10);
    memcpy(&cbuf[0], &sendPacket, sizeof(sendPacket));
    m_pcP2PSend->SetData(cbuf);

    /* Receive positions by P2P */
    const CCI_RangeAndBearingSensor::SPacket *m_psTemp;
    SimP2PPacket array[3];
    if (!m_pcP2PReceive->GetReadings().empty()) {
        int length = m_pcP2PReceive->GetReadings().size();
        for (int i = 0; i < length; i++) {
            m_psTemp = &(m_pcP2PReceive->GetReadings()[i]);
            SimP2PPacket receivedPacket = *reinterpret_cast<const SimP2PPacket *>(m_psTemp->Data.ToCArray());
            array[i] = receivedPacket;
        }
    }

    /* 30% battery check */
    if (m_pcBattery->GetReading().AvailableCharge <= BATTERY_30 && !batteryLow) {
        batteryLow = true;
        firstCallToLand = true;
        m_eState = STATE_RETURN_TO_BASE;
        LOG << "Battery level low" << std::endl;
    }

    switch (m_eState) {

        case STATE_TAKE_OFF: {
            socketInfo[SOCKET_STATE] = TAKEOFF;
            if (!batteryLow) {
                if (TakeOff(DRONE_HEIGHT, &commander, pointer)) {
                    m_eState = STATE_EXPLORE;
                }
            } else {
                m_eState = STATE_WAITING_ON_GROUND;
            }
            break;
        }

        case STATE_EXPLORE: {
            socketInfo[SOCKET_STATE] = IN_MISSION;
            CFVector dronePositions = commander.GetPosition(pointer);
            if (array != NULL && droneExplore.exploration_state == EXPLORE) {
                for (int j = 0; j < 3; j++) {
                    float distance =
                        sqrt(pow(array[j].x - dronePositions.x, 2) +
                             pow(array[j].y - dronePositions.y, 2)) *
                        METER_TO_CM;
                    /* Verify the distance between drones */
                    if (distance < DRONE_DISTANCE && array[j].id > 0 && array[j].id < 5 && id != array[j].id) {
                        if (abs(dronePositions.x - array[j].x) >
                            abs(dronePositions.y - array[j].y)) {
                            if ((float)dronePositions.x > array[j].x) {
                                droneExplore.direction = LEFT;
                            } else {
                                droneExplore.direction = RIGHT;
                            }
                        } else {
                            if ((float)dronePositions.y > array[j].y) {
                                droneExplore.direction = BACKWARD;
                            } else {
                                droneExplore.direction = FORWARD;
                            }
                        }
                        droneExplore.direction = (enum CFDirection)((droneExplore.direction + 
                            (int)round(commander.GetAngle(pointer, true) / (M_PI / 2))) % NUMBER_OF_QUADRANTS);
                        MoveInDirection(0.3, droneExplore.direction, &commander, pointer);
                    }
                }
            }
            droneExplore = Explore(droneExplore, &commander, pointer);
            break;
        }

        case STATE_LANDING: {
            socketInfo[SOCKET_STATE] = LANDING;
            if (firstCallToLand) {
                positionToLand = GetPosition(pointer);
                firstCallToLand = false;
            }

            if (Land(positionToLand, &commander, pointer)) {
                m_eState = STATE_WAITING_ON_GROUND;
            }
            break;
        }

        case STATE_WAITING_ON_GROUND: {
            socketInfo[SOCKET_STATE] = STANDBY;
            break;
        }

        case STATE_RETURN_TO_BASE: {
            socketInfo[SOCKET_STATE] = RETURN_TO_BASE;
            if (ReturnToBase(&droneReturnToBase, &commander, pointer)) {
                m_eState = STATE_LANDING;
            }
            break;
        }

        default:
            LOGERR << "[BUG] Unknown robot state: " << m_eState << std::endl;
            break;
    }
}

/****************************************/
/****************************************/

void CCrazyflieSim::Reset() {}

/****************************************/
/****************************************/

void CCrazyflieSim::socketReceive() {
    if (sendCounter < (TICKS_PER_SECOND / 4)) {
        sendCounter++;
        return;
    }

    void *pointer = static_cast<void *>(sensors);
    CFVector position = commander.GetPosition(pointer);

    socketInfo[SOCKET_ID] = DRONE_IDS[GetId()];

    auto iterDistRead = sensors->m_pcDistance->GetReadingsMap().begin();
    CFRangingDeck walls = {
        (iterDistRead++)->second,
        (iterDistRead++)->second,
        (iterDistRead++)->second,
        (iterDistRead)->second,
    };

    double oldPosx = socketInfo[SOCKET_POS_X];
    double oldPosy = socketInfo[SOCKET_POS_Y];
    socketInfo[SOCKET_BATTERY] = (int)(m_pcBattery->GetReading().AvailableCharge * 100);
    socketInfo[SOCKET_POS_X] = (int)(position.x * METER_TO_MM);
    socketInfo[SOCKET_POS_Y] = (int)(position.y * METER_TO_MM);
    socketInfo[SOCKET_POS_Z] = (int)(position.z * METER_TO_MM);
    socketInfo[SOCKET_WALL_FRONT] = (int)(walls.front * CM_TO_MM);
    socketInfo[SOCKET_WALL_LEFT] = (int)(walls.left * CM_TO_MM);
    socketInfo[SOCKET_WALL_BACK] = (int)(walls.back * CM_TO_MM);
    socketInfo[SOCKET_WALL_RIGHT] = (int)(walls.right * CM_TO_MM);
    if(m_eState != STATE_EXPLORE) {
        socketInfo[SOCKET_WALL_FRONT] = -2;
        socketInfo[SOCKET_WALL_LEFT] = -2;
        socketInfo[SOCKET_WALL_BACK] = -2;
        socketInfo[SOCKET_WALL_RIGHT] = -2;
    }
    socketInfo[SOCKET_ANGLE] = (int)(commander.GetAngle(pointer, true) * 1000);

    double speed = 0;
    if(abs(oldPosx) < MAX_POSITION && abs(oldPosy) < MAX_POSITION && socketInfo[1] != 0) {
        speed = (sqrt(
            pow((oldPosx - (double)socketInfo[SOCKET_POS_X]), 2) + pow((oldPosy - (double)socketInfo[SOCKET_POS_Y]), 2)
        )) * 4.0;
    }
    else{
        speed = 0;
    }

    socketInfo[SOCKET_SPEED] = (int)(speed * 1000.0);
    sendCounter = 0;

    string message;
    message = to_string(socketInfo[SOCKET_ID]) + ":" + to_string(socketInfo[SOCKET_STATE]) + ":" +
              to_string(socketInfo[SOCKET_SPEED]) + ":" + to_string(socketInfo[SOCKET_BATTERY]) + ":" +
              to_string(socketInfo[SOCKET_POS_X]) + ":" + to_string(socketInfo[SOCKET_POS_Y]) + ":" +
              to_string(socketInfo[SOCKET_POS_Z]) + ":" + to_string(socketInfo[SOCKET_WALL_FRONT]) + ":" +
              to_string(socketInfo[SOCKET_WALL_LEFT]) + ":" + to_string(socketInfo[SOCKET_WALL_BACK]) + ":" +
              to_string(socketInfo[SOCKET_WALL_RIGHT]) + ":" + to_string(socketInfo[SOCKET_ANGLE]);

    char buffer[BUFFER_SIZE] = {0};
    send(connectedSocket, message.c_str(), message.length(), 0);
    int valread;
    valread = read(connectedSocket, buffer, BUFFER_SIZE);
    string command(buffer);
    if (command == "takeOff") {
        m_pcPropellers->SetAbsoluteYaw(CRadians::ZERO);
        m_eState = STATE_TAKE_OFF;
        LOG << command << std::endl;
    } else if (command == "land") {
        firstCallToLand = true;
        m_eState = STATE_LANDING;
        LOG << command << std::endl;
    } else if (command == "returnToBase") {
        droneReturnToBase = {
            ROTATING,
            ROTATING,
            0,
            0,
            {0, 0, 0},
            droneReturnToBase.basePosition,
            droneReturnToBase.wallPositions
        };
        firstCallToLand = true;
        m_eState = STATE_RETURN_TO_BASE;
        LOG << command << std::endl;
    }
}

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
REGISTER_CONTROLLER(CCrazyflieSim, "crazyflie_sim")
