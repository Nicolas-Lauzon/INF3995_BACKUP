import unittest
from baseStation import BaseStation

class BaseStationTest(unittest.TestCase):

    def test_calculateSpeed(self):
        "Should calculate speed of drone"
        value = BaseStation().calculateSpeed(2, 4, 4)
        self.assertEqual(6, value)

    def test_convertBatteryVoltageNormal(self):
        "Should find correct battery value"
        value = BaseStation().convertBatteryVoltage(3.75)
        self.assertEqual(25, value)

    def test_convertBatteryVoltageNotNormal(self):
        "Should return 0 because of bad voltage value"
        value = BaseStation().convertBatteryVoltage(2)
        self.assertEqual(0, value)
