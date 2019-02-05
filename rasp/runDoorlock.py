# -*- coding: utf8 -*-
import time
import RPi.GPIO as GPIO
import _thread
 
from doorlock.controller import beep, beeps, learnUid, checkAccess, checkSchedule
from app.models.events import EventTypesEnum, Event
from doorlock.boardModels import BoardModels

boardModel = BoardModels.V3.value
# Start RFid Sensor Module and others in/out
boardModel.setup()

try:
    lastUid = None
    lastUidTime = None
    lastDBPingTime = time.time()

    print('Bring RFID card closer...')
    while True:
        if boardModel.isProgramButtonPushed():
            beeps(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed(); boardModel.blinkActivityLed()
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
                    beep()
                else:
                    print("Repeated.")
                    beeps()
            else:
                if checkAccess(uid, EventTypesEnum.IN):
                    _thread.start_new_thread(boardModel.openDoor, ())
                    beep()
                else:
                    beeps()
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

