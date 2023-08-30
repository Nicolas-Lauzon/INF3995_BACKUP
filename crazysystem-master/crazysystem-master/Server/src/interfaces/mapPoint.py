from interfaces.position import Position
from constants import constants
"""
This file creates an object MapPoint which is just a point of a wall
that the ranging deck on the drones has sensed. This object is then
sent to the UI to be put in the map.
"""

class MapPoint(Position):
    
    def __init__(self, coordX, coordY):
        if (
            (not (constants.MIN_COORD <= coordX <= constants.MAX_COORD)) or 
            (not (constants.MIN_COORD <= coordY <= constants.MAX_COORD))
        ): 
            raise Exception('Coordinates must be between 0 and 500.')

        Position.__init__(self, coordX, coordY)

    def toJSON(self):
        return {
            "coordX": self.coordX,
            "coordY": self.coordY,
        }
