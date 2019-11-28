import time
import RPi.GPIO as GPIO

from datetime import datetime

from app import db

from app.models.keyrings import Keyring, UserTypesEnum
from app.models.schedules import Schedule, DaysOfWeekEnum
from app.models.events import Event

def checkAccessType(userType):
    db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events

    now = datetime.now()
    dayOfWeek = list(DaysOfWeekEnum)[now.weekday()].name

    if userType != UserTypesEnum.STUDENT:
        return True
    else:
        if Schedule.query.count() > 0:
            if (Schedule.query
                .filter(Schedule.userType == userType)
                .filter(Schedule.dayOfWeek == dayOfWeek)
                .filter(Schedule.beginTime <= now.time())
                .filter(Schedule.endTime >= now.time())
                .count()) > 0:
                return True
            else:
                return None
        else:
            return True

def checkSchedule(uid):
    db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events

    keyring = Keyring.query.filter(Keyring.uid == uid).first()
    if keyring != None:
        return keyring if checkAccessType(keyring.userType) else None
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
                    uid if uid else "00000000",
                    eventType,
                    time
            )
    event.add(event)
    return event

def checkAccess(uid):
    keyring = checkSchedule(uid)
    if keyring:
        print('UID: %s ALLOWED \a' % uid)
    else:
        print('UID: %s not allowed \a\a' % uid)
    return keyring


