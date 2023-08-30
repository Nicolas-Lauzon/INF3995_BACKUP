import unittest
from webRouting_test import WebRoutingTest
from socketClient_test import SocketClientTest
from interfaces.mapPoint_test import MapPointTest
from mapPointSaver_test import MapPointSaverTest
from socketSimulation_test import socketSimulationTest
from mapHelper_test import MapHelperTest
from customLogger_test import CustomLoggerTest
from baseStation_test import BaseStationTest

"""
This file is the main unit test file. It controls the execution of all unit
tests for our backend.
"""

def suite():
    "Test suite for backend unit tests"
    suite = unittest.TestSuite()
    suite.addTests(
        unittest.TestLoader().loadTestsFromTestCase(WebRoutingTest)
    )
    suite.addTests(
        unittest.TestLoader().loadTestsFromTestCase(SocketClientTest)
    )
    suite.addTests(
        unittest.TestLoader().loadTestsFromTestCase(MapPointTest)
    )
    suite.addTests(
        unittest.TestLoader().loadTestsFromTestCase(MapPointSaverTest)
    )
    suite.addTest(
        unittest.TestLoader().loadTestsFromTestCase(MapHelperTest)
    )
    suite.addTest(
        unittest.TestLoader().loadTestsFromTestCase(socketSimulationTest)
    )
    suite.addTest(
        unittest.TestLoader().loadTestsFromTestCase(CustomLoggerTest)
    )
    suite.addTest(
        unittest.TestLoader().loadTestsFromTestCase(BaseStationTest)
    )

    return suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())
