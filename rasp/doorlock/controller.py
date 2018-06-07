import pygame
import os
import time
import RPi.GPIO as GPIO
import _thread
from datetime import datetime

from app.models.keyrings import Keyring
from app.models.schedules import Schedule
from app.models.events import Event
from app.models.enums import *

LOCK_RELAY_DELAY = 5 #seconds

def openDoor(pin):
    print("Opening the lock")
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, 1)
    time.sleep(LOCK_RELAY_DELAY)
    GPIO.output(pin, 0)

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

def learnUid(uid):
    keyring = Keyring.query.filter(Keyring.uid == uid).first()
    if keyring != None:
       return False

    keyring = Keyring(
                       uid,
                       -1,
                       "PROFESSOR",
                       time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
              )
    keyring.add(keyring)
    return True

def saveEvent(uid, eventType, time):
    event = Event(
                    uid,
                    eventType,
                    time
            )
    event.add(event)

def checkAccess(uid, eventType, pin):
    allowed = checkSchedule(uid)
    if allowed:
        print('UID: %s ALLOWED \a' % uid)
        _thread.start_new_thread(openDoor, (pin,))
        saveEvent(uid, eventType, datetime.now())
    else:
        print('UID: %s not allowed \a\a' % uid)
    return allowed


def beep():
    pygame.init()

    pygame.mixer.music.load(os.path.dirname(__file__) + "/beep.wav")
    pygame.mixer.music.play()

def beeps():
    pygame.init()

    pygame.mixer.music.load(os.path.dirname(__file__) + "/beeps.wav")
    pygame.mixer.music.play()

