from threading import Lock
from datetime import datetime
from socketClient import sendLog
import os
from constants import constants

"""
This file is our custom logger to log all information about the drones,
their positions, the ranging deck, and to save all the infomation in .log files
on the host PC.
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

class CustomLogger(metaclass=SingletonMeta):
    myLogFile = None
    infoTypes = ["id", "state", "speed", "battery", "position x", "position y", "position z", "range front", "range left", "range back", "range right", "yaw"]

    def __init__(self):
        logFileName = "/LOGS/"+str(datetime.now().replace(microsecond = constants.ZERO))+".log"
        dir = os.path.dirname(__file__)
        filename = os.path.join(dir, logFileName)
        self.myLogFile = open(filename, "wb", buffering = constants.ZERO)

    def logPoints(self, data):
        logToWrite = str(datetime.now()) + ' (UTC)' + ' - Info - '
        for arrayPosition in range (0, len(self.infoTypes)):
            logToWrite += self.infoTypes[arrayPosition] + " = " + str(data[arrayPosition]) + " "
        logToWrite += "\n"
        self.myLogFile.write(logToWrite.encode())
        sendLog(logToWrite)

    def logInfo(self, message):
        logToWrite = str(datetime.now()) + ' (UTC)' + ' - Info - ' + str(message) + "\n"
        self.myLogFile.write(logToWrite.encode())
        sendLog(logToWrite)
    
    def logErrors(self, error):
        logToWrite = str(datetime.now().time()) + ' (UTC)' + ' - Error - ' + str(error) + "\n"
        self.myLogFile.write(logToWrite.encode())
        sendLog(logToWrite)

    def logWarning(self, warning):
        logToWrite = str(datetime.now()) + ' (UTC)' + ' - Warning - ' + str(warning) + "\n"
        self.myLogFile.write(logToWrite.encode())
        sendLog(logToWrite)

    