# -*- coding: utf8 -*-
import time
from datetime import datetime
import argparse
import _thread
from subprocess import call

import config
from doorlock.controller import learnUid, checkAccess, checkSchedule, checkAccessType, saveEvent
from app.models.events import EventTypesEnum, Event
from app.models.keyrings import UserTypesEnum
from doorlock.boardModels import BoardModels

parser = argparse.ArgumentParser(description='Use runDoorlock.py alone or with paramater to override config.py')
parser.add_argument('-r', dest='roomName', help='Specify room code (may be room short name) like "-r E003"')
parser.add_argument('-v', dest='boardVersion', type=int, choices=range(1,5), help='Specify board version, with pins definition "-v 2|3|4"')
parser.add_argument('-d', dest='relayDelay', type=float,  help='Specify delay time, in seconds, to keep relay door active "-d .5" for half second')
parser.add_argument('-o', dest='doorOpenedAlertDelay', type=int,  help='Specify delay time, in seconds, to alert door opened')
args = parser.parse_args()

try:
    config.ROOM_NAME = args.roomName if args.roomName != None else config.ROOM_NAME
    config.BOARD_VERSION = args.boardVersion if args.boardVersion != None else config.BOARD_VERSION
    config.RELAY_DELAY = args.relayDelay if args.relayDelay != None else config.RELAY_DELAY
    config.DOOR_OPENED_ALERT_DELAY = args.doorOpenedAlertDelay if args.doorOpenedAlertDelay != None else config.DOOR_OPENED_ALERT_DELAY
except:
    pass

boardModel = list(BoardModels)[config.BOARD_VERSION-1].value
# Start RFid Sensor Module and others in/out
boardModel.setup()
locked = True

def toggleDoor(keyring):
    global locked
    locked = not locked
    #Lucio 20190501: Only magnetic lock is able to be kept opened
    if config.RELAY_DELAY > 1:
        if locked:
            boardModel.lock()
        else:
            boardModel.unlock()
    else:
        if not locked:
            openDoor(keyring)

    # Register IN or OUT events, the openDoor() above already register IN events!
    if config.RELAY_DELAY >1 or locked:
        saveEvent(keyring.uid, EventTypesEnum.OUT if locked else EventTypesEnum.IN, datetime.now())

    for x in range(0, 3):
        boardModel.beepOk() if not locked else boardModel.beepNoOk()
        boardModel.blinkActivityLed()

def openDoor(keyring):
    if locked or config.RELAY_DELAY <= 1:
        boardModel.unlock()
        time.sleep(config.RELAY_DELAY)
        boardModel.lock()
    else:
        boardModel.beepOk()

    saveEvent(keyring.uid if keyring else None, EventTypesEnum.IN, datetime.now())

def openByCommandButtonForStudent():
    if boardModel.isLightOn() or not locked:
        openDoor(None)
    else:
        boardModel.beepNoOk()

def detectDoorOpened():
    global lastDoorOpenTime
    if boardModel.isDoorOpened():
        lastDoorOpenTime = time.time()
    else:
        lastDoorOpenTime = None

try:
    lastUid = None
    lastUidTime = None
    lastKeyring = None
    lastDBPingTime = time.time()
    lastDoorOpenTime = None
    lastDoorOpenEvent = None

    boardModel.setCommandButtonCallback(openByCommandButtonForStudent)
    boardModel.setDoorSensorCallback(detectDoorOpened)
    boardModel.beepOk();boardModel.beepNoOk();boardModel.beepOk()
    
    print('Bring RFID card closer...')
    while True:
        if lastDoorOpenTime and (time.time() - lastDoorOpenTime > config.DOOR_OPENED_ALERT_DELAY):    
            boardModel.beepNoOk(); boardModel.blinkActivityLed()
            if not lastDoorOpenEvent:
                lastDoorOpenEvent = saveEvent(lastKeyring.uid if lastKeyring else None, EventTypesEnum.DOOR_OPENED, datetime.now())
        else:
            lastDoorOpenEvent = None

        if boardModel.isProgramButtonPushed() and boardModel.isCommandButtonPushed():
            boardModel.beepOk();boardModel.beepNoOk();boardModel.beepOk()
            call("sudo shutdown now", shell=True)

        if boardModel.isProgramButtonPushed():
            boardModel.beepNoOk(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed()
            print('Bring RFID card closer to learn for local access...')

        boardModel.blinkActivityLed()
        if not locked:
            boardModel.blinkActivityLed()

        # Read keyring UID
        uid = boardModel.rfidReader.readUid()
        if uid and (lastUid != uid or time.time()-lastUidTime>5): #Check lastUid to block repeated reading
            print('UID: %s' % uid)
            # Mark last uid
            lastUid = uid
            lastUidTime = time.time()

            if boardModel.isProgramButtonPushed():
                print("Learning %s..." % uid)
                if learnUid(uid):
                    print("Learned")
                    boardModel.beepOk()
                else:
                    print("Repeated.")
                    boardModel.beepNoOk()
                    boardModel.beepNoOk()
            else:
                keyring = checkAccess(uid)
                if keyring:
                    lastKeyring = keyring
                    #Lucio 20190501: Keep door opened when a professor open it
                    if keyring.userType == UserTypesEnum.PROFESSOR:
                        _thread.start_new_thread(toggleDoor, (keyring,))
                    else:
                        #Lucio 20190516: Allow employ closes an opened door
                        if keyring.userType == UserTypesEnum.EMPLOYEE and not locked:
                            _thread.start_new_thread(toggleDoor, (keyring,))
                        else:
                            _thread.start_new_thread(openDoor, (keyring,))
                else:
                    boardModel.beepNoOk()
            print("Ready...")
        else:
            time.sleep(.25)

         #Lucio 20190131: Keep DB Connection alive to avoid SessionClosed after long inactive time
        if((time.time() - lastDBPingTime)>300):
            lastDBPingTime = time.time()
            Event.query.get(1)

except KeyboardInterrupt:
    # Check if user press CTRL+C
    print('\nEnd of program.')
