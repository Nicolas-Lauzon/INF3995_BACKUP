from webRouting import webApp
from socketClient import appSocket
from interfaces.drone import Drone
import sys
import cflib
from threading import Thread
from socketSimulation import SocketSimulation
from customLogger import CustomLogger
import traceback
from baseStation import BaseStation
from constants import constants

"""
This file is our main. It controls everything, from the sockets, to the
RESTful API, and to connect to either the ARGoS simulation or the real drone(s).
It also controls all the different threads that we need to make evertything
work.
"""

def flaskThread():
    webApp.run(host=constants.CLIENT_HOST, port=constants.CLIENT_PORT, debug=False, use_reloader=False)

def socketThread():
    appSocket.run(host=constants.CLIENT_HOST, port=constants.SOCKET_CLIENT_PORT, debug=False, use_reloader=False)

def socketSimThread():
    simSocket = SocketSimulation()
    simSocket._connect()
    
def startSimulation():
    Thread(target=socketSimThread).start()
        
def startDrones():
    logger = CustomLogger()
    try:
        cflib.crtp.init_drivers(enable_debug_driver=False)
        available = []
        baseNumber = constants.CRAZYFLIE_ADDRESS
        uriStart = constants.ADDRESS_HEADER
        if (Drone.droneCount != -1):
            for i in range (0, Drone.droneCount):
                droneNumber = baseNumber + 1
                baseNumber = droneNumber
                uriDrone = hex(droneNumber).upper()[2:]
                uri = uriStart + uriDrone
                available.insert(i, uri)
            baseStation = BaseStation()
            baseStation.setUp(available)
        else:
            logger.logWarning('The amount of drones is not specified.')
    except :
        logger.logErrors(traceback.format_exc())
        
if __name__ == "__main__":
    Thread(target=flaskThread).start()
    Thread(target=socketThread).start()

    if (sys.argv[1] == 'sim'):
        startSimulation()
    if (sys.argv[1] == 'drone'):
        print('Backend in drone mode')
    else:
        print('Arg isn\'t correct, please enter \'sim\' for the simulation or \'drone\' for the real drones')
