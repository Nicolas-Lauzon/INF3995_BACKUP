import socket
from threading import Lock, Thread
import traceback
from constants import constants as const
from mapHelper import MapHelper
from interfaces.drone import Drone
from socketClient import sendMapPoints, sendDroneData, sendDronePosition
from customLogger import CustomLogger
from enums.commands import Commands

"""
This file creates the socket that we use to send and receive info to the
ARGoS simulation.
The methods allow us to send all the data we need to the simulation and to
receive the info that we need to show in our UI (including saving some
information).
"""

class SingletonMeta(type):
    _instances = {}
    _lock: Lock = Lock()

    def __call__(cls, *args, **kwargs):

        with cls._lock:

            if cls not in cls._instances:
                instance = super().__call__(*args, **kwargs)
                cls._instances[cls] = instance
        return cls._instances[cls]

class SocketSimulation(metaclass=SingletonMeta):
    client = []
    addr = None
    socket = None
    messageToDrone = []
    receivedData = []
    mapHelper = MapHelper()
    logger = None
    
    def _connect(self):
        self.socket.listen()
        print("socket is listening")
        clientNumber = const.ZERO
        while True:
            self.client.append(None)
            self.client[clientNumber], self.addr = self.socket.accept()
            print('Got connection from', self.addr)
            Thread(target=self._receiveMessage, args=(clientNumber,)).start()
            clientNumber += 1
     
    def __init__(self):
        self.message = "empty"
        self.host = const.SIM_SOCKET_HOST
        self.port = const.SIM_SOCKET_PORT
        self.logger = CustomLogger()
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print("Socket successfully created")
            self.socket.bind((const.SIM_SOCKET_HOST, self.port))
            print("socket binded to %s" % (self.port))
        except :
            self.logger.logErrors(traceback.format_exc())

    def _saveReceivedData(self, clientNumber: int):
        try:
            for currentSensor in range(const.FIRST_SENSOR, const.LAST_SENSOR):
                if (self.receivedData[clientNumber][const.TABLE_POSITION_DISTANCE_FRONT + currentSensor] > 0):
                    mapPoint = self.mapHelper.computeMapPoint(
                        self.receivedData[clientNumber][const.TABLE_POSITION_YAW] + (currentSensor * (const.PI / 2)),
                        self.receivedData[clientNumber][const.TABLE_POSITION_DISTANCE_FRONT + currentSensor],
                        self.receivedData[clientNumber][const.TABLE_POSITION_POSX],
                        self.receivedData[clientNumber][const.TABLE_POSITION_POSY],
                    )
                    if(mapPoint):
                        sendMapPoints([mapPoint])
            sendDroneData(Drone(
                self.receivedData[clientNumber][const.TABLE_POSITION_ID],
                self.receivedData[clientNumber][const.TABLE_POSITION_BATTERY],
                self.receivedData[clientNumber][const.TABLE_POSITION_STATE],
                self.receivedData[clientNumber][const.TABLE_POSITION_SPEED],
            ))
            dronePosition = self.mapHelper.computeMapPoint(
                0,
                0,
                self.receivedData[clientNumber][const.TABLE_POSITION_POSX],
                self.receivedData[clientNumber][const.TABLE_POSITION_POSY]
            )
            sendDronePosition(self.receivedData[clientNumber][const.TABLE_POSITION_ID], dronePosition)
        except :
            self.logger.logErrors(traceback.format_exc())

    def _transformStringToUsableValue(self, splitValues, clientNumber):
        try:
            self.logger.logPoints(splitValues)
            self.receivedData[clientNumber][const.TABLE_POSITION_ID] = splitValues[const.TABLE_POSITION_ID]
            self.receivedData[clientNumber][const.TABLE_POSITION_STATE] = int(splitValues[const.TABLE_POSITION_STATE])
            for i in range(2, len(self.receivedData[clientNumber])):
                self.receivedData[clientNumber][i] = float(splitValues[i])
            self.receivedData[clientNumber][const.TABLE_POSITION_YAW] /= const.YAW_BACK_TO_FLOAT
            self.receivedData[clientNumber][const.TABLE_POSITION_SPEED] = round(
                (self.receivedData[clientNumber][const.TABLE_POSITION_SPEED] / const.SPEED_BACK_TO_FLOAT),
                const.ROUND_TEN_THOUSANDTH)
            self._saveReceivedData(clientNumber)
        except:
            self.logger.logErrors(traceback.format_exc())

    def _receiveMessage(self, clientNumber):
        try:
            self.receivedData.append(const.RECEIVED_DATA_INITIAL_VALUE)
            self.messageToDrone.append("None")
            while True:
                receivedMessage = self.client[clientNumber].recv(1024).decode('utf-8')
                encodedMessage = str.encode(self.messageToDrone[clientNumber])
                self.messageToDrone[clientNumber] = "None"
                self.client[clientNumber].send(encodedMessage)
                splitValues = receivedMessage.split(":")
                if(len(splitValues) == const.RECEIVED_MESSAGE_LENGTH):
                    self._transformStringToUsableValue(splitValues, clientNumber)
        except :
            self.logger.logErrors(traceback.format_exc())

    def _sendData(self, command: Commands):
        try:
            for i in range(0, len(self.messageToDrone)):
                self.messageToDrone[i] = command.value
        except :
            self.logger.logErrors(traceback.format_exc())

    def _disconnectSocket(self):
        try:
            self.socket.close()
        except :
            self.logger.logErrors(traceback.format_exc())
