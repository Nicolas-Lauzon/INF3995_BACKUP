import unittest
from unittest import mock
from webRouting import webApp

"""
webRouting.py unit test file.
"""

class WebRoutingTest(unittest.TestCase):

    def setUp(self):
        self.webApp = webApp.test_client()
        self.webApp.testing = True

    def test_takeoff(self):
        "Drone take off"
        result = self.webApp.post('/takeoff')
        self.assertEqual(result.data, b'{"text": "Taking off."}')

    def test_land(self):
        "Drone land command"
        result = self.webApp.post('/land')
        self.assertEqual(result.data, b'{"text": "Landing."}')

    def test_softwareUpdate(self):
        "Drone software update"
        result = self.webApp.post('/softwareUpdate')
        self.assertEqual(result.data, b'{"text": "Cannot update in simulation"}')

    def test_returnToBase(self):
        "Drone return to base"
        result = self.webApp.post('/returnToBase')
        self.assertEqual(result.data, b'{"text": "Returning to base."}')

    def test_connect(self):
        "Try connecting to drones"
        result = self.webApp.post('/connect?1=2')
        self.assertEqual(result.data, b'{"text": "Cannot connect in simulation"}')

    def test_refreshPoints(self):
        "Refresh created points"
        result = self.webApp.get('/refreshPoints')
        self.assertEqual(result.data, b'[]')
