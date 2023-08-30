import os
import traceback
import csv
from threading import Lock
from typing import List
from interfaces.mapPoint import MapPoint
from customLogger import CustomLogger

"""
This file saves all of the points on the map in a CSV file so that
we can reload the page and continue to see the map (or the last map generated).
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

class MapPointSaver(metaclass=SingletonMeta):
    myFile = None
    csvFileName = None
    logger = None

    def __init__(self):
        self.logger = CustomLogger()
        try:
            logFileName = "/LOGS/savedMapPoint.csv"
            dir = os.path.dirname(__file__)
            self.csvFileName = os.path.join(dir, logFileName)
            self.myFile = open(self.csvFileName, "w+")
            self.myFile.truncate(0)
            self.myFile.close()
            self.myFile = open(self.csvFileName, mode='a')
        except :
            self.logger.logErrors(traceback.format_exc())
            
    def csvWriter(self, data: List[MapPoint]):
        try:
            fieldNames = ["posx", "posy"]
            writer = csv.DictWriter(self.myFile, fieldnames = fieldNames)
            for mapPoint in data:
                posx = mapPoint.coordX
                posy = mapPoint.coordY
                if posx != None:
                    writer.writerow({"posx": posx, "posy": posy})
            self.myFile.flush()
        except :
            self.logger.logErrors(traceback.format_exc())

    def readFromFile(self):
        try:
            freader = open(self.csvFileName, mode='r')
            reader = csv.reader(freader)
            points = []
            for line in reader:
                point = MapPoint(float(line[0]), float(line[1]))
                points.append(point.toJSON())  
            freader.close()
            return points
        except :
            self.logger.logErrors(traceback.format_exc())
        
