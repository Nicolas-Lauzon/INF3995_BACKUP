from flask import Flask, request
from flask_restful import Api
from flask_jsonpify import jsonify
from flask_cors import CORS
from interfaces.drone import Drone
from baseStation import BaseStation
from socketSimulation import SocketSimulation
import sys
import threading
from mapPointSaver import MapPointSaver
from enums.commands import Commands
from constants import constants as const
import importlib
import cflib

"""
This file is a simple RESTful API to allow our angular web page to send commands
to the backend (then to the drones). Each route is here for the different
commands that our UI can do (takeoff, land, software update, return to base,
connect) and the info needed when we reload the page (refreshPoints).
"""

webApp = Flask(__name__)

socketSimulation = None

isSimulation = False

sendResponse = threading.Event()

# Tells if the server is running for the sim or for the drones
if sys.argv[1] == 'sim':
    socketSimulation =  SocketSimulation()
    isSimulation = True
    
CORS(webApp)

def sendCommandToRightClass(command, textToReturn):
    if(isSimulation):
        socketSimulation._sendData(command)
    else:
        baseStation = BaseStation()
        baseStation.communication(command, 1)
    return jsonify({'text': textToReturn})

@webApp.route("/refreshPoints", methods=['GET'])
def sendPointsAtStart():
    mapPoints = MapPointSaver()
    pointsToSend = mapPoints.readFromFile()
    return jsonify(pointsToSend)

@webApp.route("/takeoff", methods=['POST'])
def takeoff():
    return sendCommandToRightClass(Commands.TAKE_OFF, 'Taking off.')

@webApp.route("/land", methods=['POST'])
def land():
    return sendCommandToRightClass(Commands.LAND, 'Landing.')

@webApp.route("/softwareUpdate", methods=['POST'])
def softwareUpdate():
    if(isSimulation):
        return jsonify({'text': 'Cannot update in simulation'})

    rqst = request.args.to_dict()
    if rqst == {}:
        return jsonify({'text': 'Not successful.'})
    
    cflib.crtp.init_drivers(enable_debug_driver=False)
    droneId = next(iter(rqst))

    filename = rqst.get(droneId)
    baseNumber = const.CRAZYFLIE_ADDRESS 
    uriStart = const.ADDRESS_HEADER
    droneNumber = baseNumber + int(droneId)
    uriDrone = hex(droneNumber).upper()[2:]
    uri = uriStart + uriDrone
    
    baseStation = BaseStation()
    baseStation.setUp([uri])
    value = baseStation.flash(uri, filename)
    if (value == 0):
        return '0'
    else:
        return '1'

@webApp.route("/returnToBase", methods=['POST'])
def returnToBase():
    return sendCommandToRightClass(Commands.RETURN_TO_BASE, 'Returning to base.')

@webApp.route("/connect", methods = ['POST'])
def connect():
    if(isSimulation):
        return jsonify({'text': 'Cannot connect in simulation'})
    
    startDrones = getattr(importlib.import_module('server'), 'startDrones')
    rqst = request.args.to_dict()
    command = rqst.get('1')
    Drone.droneCount = int(command)
    startDrones()
    if sendResponse.wait(45):
        sendResponse.clear()
        return '0'
    sendResponse.clear()
    return '1'
