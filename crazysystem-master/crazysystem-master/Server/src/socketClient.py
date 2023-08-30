from flask import Flask
from flask_socketio import SocketIO
from flask_jsonpify import jsonify
import traceback
from interfaces.mapPoint import MapPoint
from interfaces.drone import Drone
from typing import List
import importlib
import logging
from interfaces.dronePosition import DronePosition

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

"""
This file creates the Socket.IO TCP socket that we use to send info to the UI.
The methods allow us to send all the data we need to the UI to be shown in the
specified areas. 
"""

appSocket = Flask(__name__)
appSocket.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(appSocket, cors_allowed_origins="*")

def sendDroneData(drone: Drone):
    loggerClass = getattr(importlib.import_module('customLogger'), 'CustomLogger')
    logger = loggerClass()
    try:
        socketio.emit('drone-info', drone.toJSON())
    except :
        logger.logErrors(traceback.format_exc())

def sendMapPoints(newPoints: List[MapPoint]):
    mapPointSaverClass = getattr(importlib.import_module('mapPointSaver'), 'MapPointSaver')
    loggerClass = getattr(importlib.import_module('customLogger'), 'CustomLogger')
    logger = loggerClass()
    mapPointSaver = mapPointSaverClass()
    pointsJson = []
    for point in newPoints:
        try:
            pointsJson.append(point.toJSON())
        except Exception:
            logger.logErrors(traceback.format_exc())
    try:
        mapPointSaver.csvWriter(newPoints)
        socketio.emit('new-map-points', pointsJson)
    except :
        logger.logErrors(traceback.format_exc())

def sendLog(log):
    socketio.emit('logs', log)

def sendDronePosition(droneId, position: MapPoint):
    objectToSend = DronePosition(droneId, position)
    socketio.emit('drone-position', objectToSend.toJSON())
