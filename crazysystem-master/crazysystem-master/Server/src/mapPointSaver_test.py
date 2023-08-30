import unittest
import csv

from mapPointSaver import MapPointSaver
from interfaces.mapPoint import MapPoint
from typing import List

class MapPointSaverTest(unittest.TestCase):

    def setUp(self):
        pointSaver = MapPointSaver()
        pointSaver.myFile.close()
        pointSaver.myFile = open(pointSaver.csvFileName, "w+")
        pointSaver.myFile.truncate(0)
        pointSaver.myFile.close()
        pointSaver.myFile = open(pointSaver.csvFileName, mode='a')
    
    def test_writeToFile(self):
        "Should write coordinate to csv file when csvWriter is called"
        fakeX = 200
        fakeY = 250
        fakePoint = MapPoint(fakeX, fakeY)
        pointSaver = MapPointSaver()
        pointSaver.csvWriter([fakePoint])
        freader = open(pointSaver.csvFileName, mode='r')
        reader = csv.reader(freader)
        for line in reader:
            point = MapPoint(float(line[0]), float(line[1]))
            self.assertEqual(point.coordX, fakePoint.coordX)
            self.assertEqual(point.coordY, fakePoint.coordY)

    def test_readFile(self): 
        "Should return an array of json elements of the coordinates saved in the csv file"
        fakePoints = [MapPoint(100, 100), MapPoint(150, 150), MapPoint(200, 200), MapPoint(250, 250), MapPoint(300, 300)]
        pointSaver = MapPointSaver()
        fieldNames = ["posx", "posy"]
        writer = csv.DictWriter(pointSaver.myFile, fieldnames = fieldNames)
        for mapPoint in fakePoints:
            x = mapPoint.coordX
            y = mapPoint.coordY
            writer.writerow({"posx": x, "posy": y})
        pointSaver.myFile.flush()
        readValues = pointSaver.readFromFile()
        expectedValue = [
            {'coordX': 100, 'coordY': 100},
            {'coordX': 150, 'coordY': 150},
            {'coordX': 200, 'coordY': 200},
            {'coordX': 250, 'coordY': 250},
            {'coordX': 300, 'coordY': 300}
        ]
        self.assertEqual(readValues, expectedValue)
        
