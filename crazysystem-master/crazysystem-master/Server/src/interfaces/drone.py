"""
This file represents a basic drone information structure. When we need
to send the info to the UI, we set a new object Drone with the needed
info (id, speed, state, etc) and then send it to the UI.
"""

class Drone():
    droneCount = -1

    def __init__(self, id, batteryLevel, state = 0, speed = -1):
        self.id = id
        self.batteryLevel = batteryLevel
        self.state = state
        self.speed = speed

    def toJSON(self):
        return {
            "id": self.id,
            "batteryLevel": self.batteryLevel,
            "state": self.state,
            "speed": self.speed,
        }
