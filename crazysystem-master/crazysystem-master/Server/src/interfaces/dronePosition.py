"""
This file represents a basic drone position structure. When we need
to send the info to the UI, we set a new object DronePosition with the needed
info (id, position) and then send it to the UI.
"""

class DronePosition():

    def __init__(self, id, position):
        self.id = id
        self.position = position

    def toJSON(self):
        return {
            "id": self.id,
            "position": self.position.toJSON(),
        }
