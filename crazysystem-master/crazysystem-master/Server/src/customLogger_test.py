import unittest
from customLogger import CustomLogger

"""
customLogger.py unit test file.
"""

class CustomLoggerTest(unittest.TestCase):

    def setUp(self):
        self.customLogger = CustomLogger()

    def test_logPoints(self):
        "Should log points into log file"
        startPosition = self.customLogger.myLogFile.tell()
        fakeInfo = []
        for info in self.customLogger.infoTypes:
            fakeInfo.append(info)
        self.customLogger.logPoints(fakeInfo)
        self.assertTrue(startPosition < self.customLogger.myLogFile.tell())

    def test_logInfo(self):
        "Should log info into log file"
        startPosition = self.customLogger.myLogFile.tell()
        self.customLogger.logInfo("this drone is logging some good info")
        self.assertTrue(startPosition < self.customLogger.myLogFile.tell())

    def test_logErrors(self):
        "Should log errors into log file"
        startPosition = self.customLogger.myLogFile.tell()
        self.customLogger.logErrors("this drone is logging some errors")
        self.assertTrue(startPosition < self.customLogger.myLogFile.tell())

    def test_logWarning(self):
        "Should log warnings into log file"
        startPosition = self.customLogger.myLogFile.tell()
        self.customLogger.logWarning("this drone is warning you")
        self.assertTrue(startPosition < self.customLogger.myLogFile.tell())
