import unittest
from unittest import mock
import socket
from threading import Thread
import time
import sys
from socketSimulation import SocketSimulation
import socketClient
from enums.commands import Commands

class socketSimulationTest(unittest.TestCase) :
    def setUp(self):
        self.socketSimulation = SocketSimulation()
        
    def test_transformStringToUsableValue(self):
        "Transform received string to drone values"
        data = "1:2:3:4:5:6:7:8:9:10:11:12".split(":")
        self.socketSimulation.receivedData.append([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])
        self.socketSimulation._transformStringToUsableValue(data, 0)
        self.assertEqual(self.socketSimulation.receivedData, [['1', 2, 0.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 0.012]])
    
    def test_sendData(self):
        "Send data to client"
        self.socketSimulation.messageToDrone.append(None)
        self.socketSimulation._sendData(Commands.LAND)
        for message in self.socketSimulation.messageToDrone:
            self.assertEqual(message, Commands.LAND.value)

    def tearDown(self):
        self.socketSimulation._disconnectSocket()
