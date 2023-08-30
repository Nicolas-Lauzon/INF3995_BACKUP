import unittest
from unittest import mock
from interfaces.drone import Drone
from interfaces.mapPoint import MapPoint
from interfaces.dronePosition import DronePosition
from socketClient import appSocket, sendDroneData, socketio, sendMapPoints, sendLog, sendDronePosition

"""
socketClient.py unit test file.
"""

class SocketClientTest(unittest.TestCase):

    def setUp(self):
        self.appSocket = appSocket
        self.appSocket.testing = True
        self.testClient = socketio.test_client(self.appSocket)

    def test_sendDroneData(self):
        "send drone data with socket"
        droneElement = Drone(1, 42, 0, 69)
        droneElementJson = droneElement.toJSON()
        sendDroneData(droneElement)
        received = self.testClient.get_received()
        self.assertEqual(received[0]['args'][0], droneElementJson)

    def test_sendMapPoints(self):
        "send map points with socket"
        fakeCoordX = 69
        fakeCoordY = 42
        fakePoint1 = MapPoint(fakeCoordX, fakeCoordY)
        fakePoint2 = MapPoint(fakeCoordX + 3, fakeCoordY + 3)
        fakePointList = [fakePoint1, fakePoint2]
        fakePointListJson = [fakePoint1.toJSON(), fakePoint2.toJSON()]
        sendMapPoints(fakePointList)
        received = self.testClient.get_received()
        self.assertEqual(received[0]['args'][0], fakePointListJson)
        self.assertEqual(received[0]['name'], 'new-map-points')

    def test_sendLog(self):
        "Send log with socket"
        fakeLog = 'Hello I am fake LOG'
        sendLog(fakeLog)
        received = self.testClient.get_received()
        self.assertEqual(received[0]['args'][0], fakeLog)
        self.assertEqual(received[0]['name'], 'logs')

    def test_sendDronePositions(self):
        "Send drone positions with socket"
        fakeCoordX = 69
        fakeCoordY = 42
        fakePosition = MapPoint(fakeCoordX, fakeCoordY)
        fakeDronePosition = DronePosition(1, fakePosition).toJSON()
        sendDronePosition(1, fakePosition)
        received = self.testClient.get_received()
        self.assertEqual(received[0]['args'][0], fakeDronePosition)
        self.assertEqual(received[0]['name'], 'drone-position')
