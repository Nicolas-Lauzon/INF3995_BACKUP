import unittest
from mapHelper import MapHelper
from interfaces.mapPoint import MapPoint

"""
mapHelper.py unit test file.
"""

class MapHelperTest(unittest.TestCase):

    def test_computeMapPoint(self):
        "Should return the correct MapPoint"
        mapHelper = MapHelper()
        point = mapHelper.computeMapPoint(45, 10, 5, 5)
        correctPoint = MapPoint(250.512,250.675)
        self.assertAlmostEqual(point.coordX, correctPoint.coordX, delta=0.1)
        self.assertAlmostEqual(point.coordY, correctPoint.coordY, delta=0.1)
    
    def test_checkRangeValues(self):
        "Should return -1 if the range is greater than 3500"
        mapHelper = MapHelper()
        smaller = mapHelper.checkRangeValues(1000)
        bigger = mapHelper.checkRangeValues(5000)
        self.assertEqual(smaller, 1000)
        self.assertEqual(bigger, -1)

    def test_outOfBoundsPoint(self):
        "Should return false if mapped point is outside map"
        mapHelper = MapHelper()
        result = mapHelper.computeMapPoint(0, 15000, 5000, 5000)
        self.assertFalse(result)
