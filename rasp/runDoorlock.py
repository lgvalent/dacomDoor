# -*- coding: utf8 -*-
import time
import argparse
import _thread
from subprocess import call

import RPi.GPIO as GPIO

import config
from doorlock.controller import learnUid, checkAccess, checkSchedule
from app.models.events import EventTypesEnum, Event
from doorlock.boardModels import BoardModels

parser = argparse.ArgumentParser(description='Use runDoorlock.py alone or with paramater to override config.py')
parser.add_argument('-r', dest='roomName', help='Specify room code (may be room short name) like "-r E003"')
parser.add_argument('-v', dest='boardVersion', type=int, choices=range(1,5), help='Specify board version, with pins definition "-v 2|3|4"')
parser.add_argument('-d', dest='relayDelay', type=float,  help='Specify delay time, in seconds, to keep relay door active "-d .5" for half second')
args = parser.parse_args()

try:
    config.ROOM_NAME = args.roomName if args.roomName != None else config.ROOM_NAME
    config.BOARD_VERSION = args.boardVersion if args.boardVersion != None else config.BOARD_VERSION
    config.RELAY_DELAY = args.relayDelay if args.relayDelay != None else config.RELAY_DELAY
except:
    pass

boardModel = list(BoardModels)[config.BOARD_VERSION-1].value
# Start RFid Sensor Module and others in/out
boardModel.setup()

try:
    lastUid = None
    lastUidTime = None
    lastDBPingTime = time.time()

    print('Bring RFID card closer...')
    while True:
        if boardModel.isProgramButtonPushed() and boardModel.isCommandButtonPushed():
            call("sudo shutdown now", shell=True) 

        if boardModel.isProgramButtonPushed():
            boardModel.beepNoOk(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed()
            print('Bring RFID card closer to learn for local access...')

        if boardModel.isCommandButtonPushed():
            boardModel.openDoor()

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
                if checkAccess(uid, EventTypesEnum.IN):
                    _thread.start_new_thread(boardModel.openDoor, ())
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

