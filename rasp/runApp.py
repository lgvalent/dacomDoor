import time
import argparse
from threading import Timer

from app.controllers.eventsUpdate import EventsUpdate
from app.controllers.keyringsUpdate import KeyringsUpdate
from app.controllers.schedulesUpdate import SchedulesUpdate

import config

parser = argparse.ArgumentParser(description='Use runApp.py alone or with paramater to override config.py')
parser.add_argument('-r', dest='roomName', help='Specify room code (may be room short name) like "-r E003"')
parser.add_argument('-u', dest='urlServer', help='Specify server URL with name and port like "-u http://localhost:5000"')
parser.add_argument('-d', dest='updateDelay', help='Specify delay time, in seconds, to update each buffer "-d 60"')
args = parser.parse_args()

try:
    config.ROOM_NAME = args.roomName if args.roomName != None else config.ROOM_NAME;
    config.URL_SERVER = args.urlServer if args.roomName != None else config.URL_SERVER;
    config.UPDATE_DELAY = args.updateDelay if args.roomName != None else config.UPDATE_DELAY;
except:
    pass


print ("Synchronizing local KEYRINGS, EVENTS and SCHEDULE on {} \r\nwith remote {}.".format(config.ROOM_NAME,config.URL_SERVER))

def tasks():
    #print ("Updating...")
    KeyringsUpdate().update()
    time.sleep(config.UPDATE_DELAY)
    SchedulesUpdate().update()
    time.sleep(config.UPDATE_DELAY)
    EventsUpdate().update()
    time.sleep(config.UPDATE_DELAY)
    Timer(2, tasks).start() #make loop forever

if __name__ == '__main__':
    tasks()

