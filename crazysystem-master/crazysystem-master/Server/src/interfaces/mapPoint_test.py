import unittest
from interfaces.mapPoint import MapPoint

"""
mapPoint.py unit test file.
"""

class MapPointTest(unittest.TestCase):

    def test_mapPointCreationExeption(self):
        "Should raise an exeption when creating a map point with coord over 500"
        self.assertRaises(Exception, MapPoint, 1000, 1000)

    def test_mapPointJsonConversion(self):
        "Should convert mapPoint to JSON format"
        returnedJson = MapPoint(69, 42).toJSON()
        expectedJson = {
            "coordX": 69,
            "coordY": 42,
        }
        self.assertEqual(returnedJson, expectedJson)
