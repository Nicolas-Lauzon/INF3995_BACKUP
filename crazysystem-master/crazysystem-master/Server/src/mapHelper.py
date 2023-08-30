from interfaces.position import Position
from interfaces.mapPoint import MapPoint
import math
from constants import constants

"""
This file is our map calculation class. It calculates the positions
of the walls depending on the distances from the ranging deck and the
drones position.
"""

class MapHelper():

    def __init__(
        self,
        mapLimits: MapPoint = MapPoint(constants.MAPPOINT_DEFAULT, constants.MAPPOINT_DEFAULT),
        maxDroneDistance: Position = Position(constants.POSITION_DEFAULT, constants.POSITION_DEFAULT),
    ):
        self.mapLimits = mapLimits
        self.maxDroneDistance = maxDroneDistance
    
    def computeMapPoint(self, orientation, distance, positionX, positionY):

        # Find position in robot coords
        pointX = positionX + (distance * math.cos(orientation))
        pointY = positionY + (distance * math.sin(orientation))
        # Find position in unit coords (between 0 and 1)
        unitPointX = (pointX + self.maxDroneDistance.coordX / 2) / self.maxDroneDistance.coordX
        unitPointY = (pointY + self.maxDroneDistance.coordY / 2) / self.maxDroneDistance.coordY
         # Find position in map coords
        mappedPointX = unitPointX * self.mapLimits.coordX
        mappedPointY = unitPointY * self.mapLimits.coordY
        if(
            mappedPointX > constants.MAPPEDPOINT_MAX or
            mappedPointX < constants.MAPPEDPOINT_MIN or
            mappedPointY > constants.MAPPEDPOINT_MAX or 
            mappedPointY < constants.MAPPEDPOINT_MIN
        ):

            return False

        return MapPoint(mappedPointX, mappedPointY)

    def checkRangeValues(self, value):
        if (value >= constants.MAX_RANGE_VALUE):
            return -1
        return value
