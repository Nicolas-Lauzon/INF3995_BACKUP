import logging
from threading import Lock, Thread
import cflib
import math
import traceback
from constants import constants
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.syncLogger import SyncLogger
from cflib.bootloader import Bootloader
from socketClient import sendDroneData, sendMapPoints, sendDronePosition
from interfaces.drone import Drone
from mapHelper import MapHelper
from customLogger import CustomLogger
from enums.commands import Commands

logging.basicConfig(level=logging.ERROR)

VOLTAGE_DICT = {
    4.2:  100,
    4.15: 95,
    4.11: 90,
    4.08: 85,
    4.02: 80,
    3.98: 75,
    3.95: 70,
    3.91: 65,
    3.87: 60,
    3.85: 55,
    3.84: 50,
    3.82: 45,
    3.80: 40,
    3.79: 35,
    3.77: 30,
    3.75: 25,
    3.73: 20,
    3.71: 15,
    3.69: 10,
    3.61: 5,
    3.27: 0
}

"""
This file is a singleton class that communicates with the drone(s).
It takes care of all the basic logging and commands to the Crazyflie drone(s).
all the logging is done in the "getDroneInfo" method.
To control the drone with the parameters, we use the "communication" method.
The other methods are to take care of errors and the logging data.
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

class BaseStation(metaclass=SingletonMeta):
    mapHelper = MapHelper()
    logger = None
    dronesInfos = dict()
    crazyFlie = []

    def __init__(self):
        self.logger = CustomLogger()
    
    def setUp(self, link_uri):
        self.link = link_uri
        self.bootloader = Bootloader()
        for cf in self.crazyFlie:
            cf.close_link()
            self.crazyFlie.remove(cf)
        for i in link_uri:
            newCrazyflie = Crazyflie()
            newCrazyflie.connected.add_callback(self._connected)
            newCrazyflie.disconnected.add_callback(self._disconnected)
            newCrazyflie.connection_failed.add_callback(self._connection_failed)
            newCrazyflie.connection_lost.add_callback(self._connection_lost)
            newCrazyflie.open_link(i)
            self.crazyFlie.append(newCrazyflie)
            print('Connecting to %s' % i)

    def _connected(self, link_uri):
        from webRouting import sendResponse

        stringNumber = link_uri[-2:]
        droneId = int(stringNumber, constants.BASE_16)
        self.dronesInfos[droneId] = [droneId, None, None, None, None, None, None, None, None, None, None, None]

        sendResponse.set()
        sendResponse.clear()
        
        Thread(target=self.communication(Commands.TAKE_OFF, 0)).start()
        Thread(target=self.getDroneInfo(link_uri)).start()
       

    def _basicDroneInfo_log_data(self, timestamp, data, logconf):
        id = data['droneInfo.droneNumber']
        batteryVoltage = round(data['pm.vbat'], constants.ROUND_THOUSANDTH)
        batteryLvl = self.convertBatteryVoltage(batteryVoltage)
        state = data['droneInfo.state']
        speed = self.calculateSpeed(data['stateEstimate.vx'], data['stateEstimate.vy'], data['stateEstimate.vz'])
        droneInfo = Drone(id, batteryLvl, state, speed)
        sendDroneData(droneInfo)
        self.dronesInfos[id][constants.TABLE_POSITION_STATE] = state 
        self.dronesInfos[id][constants.TABLE_POSITION_SPEED] = speed
        self.dronesInfos[id][constants.TABLE_POSITION_BATTERY] = batteryLvl
        self.sendDroneInfo()

    def _droneRangingDeck_log_data(self, timestamp, data, logconf):
        id = data['droneInfo.droneNumber']
        front = self.mapHelper.checkRangeValues(data['range.front'])
        back = self.mapHelper.checkRangeValues(data['range.back'])
        left = self.mapHelper.checkRangeValues(data['range.left'])
        right = self.mapHelper.checkRangeValues(data['range.right'])
        up = self.mapHelper.checkRangeValues(data['range.up'])
        down = self.mapHelper.checkRangeValues(data['range.zrange'])
        self.dronesInfos[id][constants.TABLE_POSITION_DISTANCE_FRONT] = front
        self.dronesInfos[id][constants.TABLE_POSITION_DISTANCE_LEFT] = left
        self.dronesInfos[id][constants.TABLE_POSITION_DISTANCE_BACK] = back
        self.dronesInfos[id][constants.TABLE_POSITION_DISTANCE_RIGHT] = right
        self.sendDroneInfo()

    def _dronePosition_log_data(self, timestamp, data, logconf):
        id = data['droneInfo.droneNumber']
        positionx = data['stateEstimate.x']
        positiony = data['stateEstimate.y'] - (0.5 * (id - 1))
        positionz = data['stateEstimate.z']
        yaw = data['stabilizer.yaw'] * (math.pi / 180)
        self.dronesInfos[id][constants.TABLE_POSITION_POSX] = positionx
        self.dronesInfos[id][constants.TABLE_POSITION_POSY] = positiony
        self.dronesInfos[id][constants.TABLE_POSITION_POSZ] = positionz
        self.dronesInfos[id][constants.TABLE_POSITION_YAW] = yaw
        self.sendDroneInfo()

    def _drone_log_error(self, logconf, msg):
        error = 'Error when logging ' + logconf.name + ': ' + msg
        print(error)
        self.logger.logErrors(error)
        
    def _connection_failed(self, link_uri, msg):
        try:
            self.crazyFlie.remove(self.getCrazyflieFromUri(link_uri))
        except:
            self.logger.logErrors(traceback.format_exc())
        finally:
            error = 'Connection to ' + link_uri + ' failed: ' + msg
            print(error)
            self.logger.logErrors(error)
        

    def _connection_lost(self, link_uri, msg):
        try:
            self.crazyFlie.remove(self.getCrazyflieFromUri(link_uri))
        except:
            self.logger.logErrors(traceback.format_exc())
        finally:
            error = 'Connection to ' + link_uri + ' lost: ' + msg
            print(error)
            self.logger.logErrors(error)

    def _disconnected(self, link_uri):
        error = 'Disconnected from ' + link_uri 
        print(error)
        self.logger.logErrors(error)

    def getCrazyflieFromUri(self, link_uri):
        for cf in self.crazyFlie:
            if cf.link_uri == link_uri:
                    return cf
        error = 'Crazyflie not found with uri: ' + link_uri
        print(error)
        self.logger.logErrors(error)
        return None

    def communication(self, command: Commands, value):
        for i in range(len(self.crazyFlie)):
            self.crazyFlie[i].param.set_value('commands.' + command.value, value)
        return

    def getDroneInfo(self, link_uri):
        # There are 3 logs because each log is limited to 26 bytes of "data", uint8 = 1 byte, float = 4, uint16 = 2.
        basicDroneInfo = LogConfig(name='basicDroneInfo', period_in_ms = constants.TIME_TO_REFRESH_MS)
        basicDroneInfo.add_variable('droneInfo.droneNumber', 'uint8_t')
        basicDroneInfo.add_variable('pm.vbat', 'float')
        basicDroneInfo.add_variable('droneInfo.state', 'uint8_t')
        basicDroneInfo.add_variable('stateEstimate.vx', 'float')
        basicDroneInfo.add_variable('stateEstimate.vy', 'float')
        basicDroneInfo.add_variable('stateEstimate.vz', 'float')

        droneRangingDeck = LogConfig(name="rangingDeck", period_in_ms = constants.TIME_TO_REFRESH_MS)
        droneRangingDeck.add_variable('droneInfo.droneNumber', 'uint8_t')
        droneRangingDeck.add_variable('range.front', 'uint16_t')
        droneRangingDeck.add_variable('range.back', 'uint16_t')
        droneRangingDeck.add_variable('range.left', 'uint16_t')
        droneRangingDeck.add_variable('range.right', 'uint16_t')
        droneRangingDeck.add_variable('range.up', 'uint16_t')
        droneRangingDeck.add_variable('range.zrange', 'uint16_t')

        dronePosition = LogConfig(name='dronePosition', period_in_ms = constants.TIME_TO_REFRESH_MS)
        dronePosition.add_variable('droneInfo.droneNumber', 'uint8_t')
        dronePosition.add_variable('stateEstimate.x', 'float')
        dronePosition.add_variable('stateEstimate.y', 'float')
        dronePosition.add_variable('stateEstimate.z', 'float')
        dronePosition.add_variable('stabilizer.yaw', 'float')

        try:
            crazyflie = self.getCrazyflieFromUri(link_uri)
            if (crazyflie is None):
                return
            crazyflie.log.add_config(basicDroneInfo)
            basicDroneInfo.data_received_cb.add_callback(self._basicDroneInfo_log_data)
            basicDroneInfo.error_cb.add_callback(self._drone_log_error)
            basicDroneInfo.start()

            crazyflie.log.add_config(droneRangingDeck)
            droneRangingDeck.data_received_cb.add_callback(self._droneRangingDeck_log_data)
            droneRangingDeck.error_cb.add_callback(self._drone_log_error)
            droneRangingDeck.start()

            crazyflie.log.add_config(dronePosition)
            dronePosition.data_received_cb.add_callback(self._dronePosition_log_data)
            dronePosition.error_cb.add_callback(self._drone_log_error)
            dronePosition.start()

        except KeyError as e:
            error = 'Could not start log config, ' + format(str(e)) + ' not found in TOC'
            print(error)
            self.logger.logErrors(error)
        except AttributeError as ee:
            error = 'Could not add attribute log config, bad configuration: ' + format(str(ee))
            print(error)
            self.logger.logErrors(error)

    def flash(self, link_uri, filename):
        value = self.rebootToBootloader(link_uri)
        
        targets = {}
        targets['stm32'] = ("fw",)
        try:
            self.bootloader.flash(str(filename), targets)
        except:
            self.logger.logErrors(traceback.format_exc())
            self.bootloader.reset_to_firmware()
            print("Error while updating. Rebooting.")
            return 1
        print("Update successful, rebooting.")
        self.bootloader.reset_to_firmware()
        self.bootloader.close()
        return value

    def rebootToBootloader(self, link_uri):      
        self.bootloader.clink = link_uri
        self.getCrazyflieFromUri(link_uri).close_link()

        try:
            success = self.bootloader.start_bootloader(warm_boot=True)
            if not success:
                print("Could not connect to bootloader")
                return 1
            else:
                print("Flashing starting...")
                return 0
        except :
            self.logger.logErrors(traceback.format_exc())
            return 1

    def sendDroneInfo(self):
        for droneId in self.dronesInfos:
            if None not in self.dronesInfos[droneId]:
                if (self.dronesInfos[droneId][1] == 1): #if the drone is flying
                    self.logger.logPoints(self.dronesInfos[droneId])
                    if (not (
                        self.dronesInfos[droneId][7] == -1 or
                        self.dronesInfos[droneId][8] == -1 or
                        self.dronesInfos[droneId][9] == -1 or
                        self.dronesInfos[droneId][10] == -1
                    )):
                        mapPointFront = self.mapHelper.computeMapPoint(
                            self.dronesInfos[droneId][11],
                            self.dronesInfos[droneId][7],
                            self.dronesInfos[droneId][4] * 1000,
                            self.dronesInfos[droneId][5] * 1000
                        )
                        mapPointBack = self.mapHelper.computeMapPoint(
                            self.dronesInfos[droneId][11] + math.pi,
                            self.dronesInfos[droneId][9],
                            self.dronesInfos[droneId][4] * 1000,
                            self.dronesInfos[droneId][5] * 1000
                        )
                        mapPointLeft = self.mapHelper.computeMapPoint(
                            self.dronesInfos[droneId][11] + (math.pi/2),
                            self.dronesInfos[droneId][8],
                            self.dronesInfos[droneId][4] * 1000,
                            self.dronesInfos[droneId][5] * 1000
                        )
                        mapPointRight = self.mapHelper.computeMapPoint(
                            self.dronesInfos[droneId][11] + (3 * math.pi / 2),
                            self.dronesInfos[droneId][10],
                            self.dronesInfos[droneId][4] * 1000,
                            self.dronesInfos[droneId][5] * 1000
                        )
                        sendMapPoints([mapPointFront, mapPointBack, mapPointLeft, mapPointRight])
                dronePosition = self.mapHelper.computeMapPoint(
                    0,
                    0,
                    self.dronesInfos[droneId][4] * 1000,
                    self.dronesInfos[droneId][5] * 1000
                )
                sendDronePosition(droneId, dronePosition)
                self.dronesInfos[droneId] = [droneId, None, None, None, None, None, None, None, None, None, None, None]

    def calculateSpeed(self, x, y, z):
        speed = math.sqrt(math.pow(x, constants.SQUARE_POWER) + math.pow(y, constants.SQUARE_POWER) \
                        + math.pow(z, constants.SQUARE_POWER))
        return round(speed, constants.ROUND_THOUSANDTH)

    def convertBatteryVoltage(self, batteryVoltage):
        for key in VOLTAGE_DICT:
            if (key <= batteryVoltage):
                return VOLTAGE_DICT[key]
        return 0
