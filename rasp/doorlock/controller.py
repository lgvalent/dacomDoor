import time
import RPi.GPIO as GPIO
import _thread
from datetime import datetime

from app.models.keyrings import Keyring
from app.models.schedules import Schedule
from app.models.events import Event
from app.models.enums import *

doorRelayPin = 12
doorRelayDelay = 5

def openDoor():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(doorRelayPin, GPIO.OUT)
    GPIO.output(doorRelayPin, 1)
    time.sleep(doorRelayDelay)
    GPIO.output(doorRelayPin, 0)

def checkSchedule(uid):
    keyring = Keyring.query.filter(Keyring.uid == uid).first()
    if keyring != None:
        now = datetime.now()
        dayOfWeek = list(DaysOfWeekEnum)[now.weekday()].name

        if(keyring.userType != UserTypesEnum.STUDENT):
            return True
        else:
            schedules = (Schedule.query
                .filter(Schedule.userType == keyring.userType)
                .filter(Schedule.dayOfWeek == dayOfWeek)
                .filter(Schedule.beginTime <= now.time())
                .filter(Schedule.endTime >= now.time())
                .all()
            )
            if schedules != []:
                return True
            else:
                return False

def saveEvent(uid, eventType, time):
    event = Event(
                    uid,
                    eventType,
                    time
            )
    event.add(event)

def checkUid(uid, eventType):
    allowed = checkSchedule(uid)
    if allowed:
        print('UID: %s ALLOWED' % uid)
        _thread.start_new_thread(openDoor, ())
        saveEvent(uid, eventType, datetime.now())
    else:
        print('UID: %s not allowed' % uid)
