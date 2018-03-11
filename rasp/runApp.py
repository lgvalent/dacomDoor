import time
import argparse

from app.controllers.eventsUpdate import EventsUpdate
from app.controllers.keyringsUpdate import KeyringsUpdate
from app.controllers.schedulesUpdate import SchedulesUpdate
import config

parser = argparse.ArgumentParser(description='Use run.py alone or with paramater [-r "rom_name"] to override config.py')
parser.add_argument('-s', dest='roomName', help='USAGE: "python run.py -r E003"')
args = parser.parse_args()

if args.roomName != None:
    try:
        config.ROOM_NAME = args.roomName;
    except:
        pass

def tasks():
    KeyringsUpdate().update()
    time.sleep(10)
    SchedulesUpdate().update()
    time.sleep(10)
    EventsUpdate().update()
    time.sleep(10)
    tasks()

if __name__ == '__main__':
    tasks()