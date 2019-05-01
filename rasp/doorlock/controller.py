import time
import RPi.GPIO as GPIO

from datetime import datetime

from app import db

from app.models.keyrings import Keyring, UserTypesEnum
from app.models.schedules import Schedule, DaysOfWeekEnum
from app.models.events import Event


def checkSchedule(uid):
    db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events

    keyring = Keyring.query.filter(Keyring.uid == uid).first()
    if keyring != None:
        now = datetime.now()
        dayOfWeek = list(DaysOfWeekEnum)[now.weekday()].name

        if keyring.userType != UserTypesEnum.STUDENT:
            return keyring
        else:
            if Schedule.query.count() > 0:
                if (Schedule.query
                .filter(Schedule.userType == keyring.userType)
                .filter(Schedule.dayOfWeek == dayOfWeek)
                .filter(Schedule.beginTime <= now.time())
                .filter(Schedule.endTime >= now.time())
                .count()) > 0:
                    return keyring
                else:
                    return None
            else:
                return keyring
    else:
        return None


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

def checkAccess(uid, eventType):
    keyring = checkSchedule(uid)
    if keyring:
        print('UID: %s ALLOWED \a' % uid)
        saveEvent(uid, eventType, datetime.now())
    else:
        print('UID: %s not allowed \a\a' % uid)
    return keyring


