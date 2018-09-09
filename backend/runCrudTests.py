from datetime import datetime

from app import db

from app.utils import response, rollback

from app.models.user import User, UserSchema, UserTypesEnum
from app.models.room import Room, RoomSchema, RoomUser
from app.models.schedule import Schedule, ScheduleSchema, DaysOfWeekEnum
from app.models.event import Event, EventSchema

schedules = Schedule.query.all()
print ("SchedulesList: {} ...\n".format(len(schedules)))
Schedule.query.delete()

roomsUsers = RoomUser.query.all()
print ("RoomsUseresList: {} ...\n".format(len(roomsUsers)))
RoomUser.query.delete()

rooms = Room.query.all()
print ("RoomsList: {} ...\n".format(len(rooms)))
Room.query.delete()

events = Event.query.all()
print ("EventsList: {} ...\n".format(len(events)))
Event.query.delete()

users = User.query.all()
print ("UsersList: {} ...\n".format(len(users)))
User.query.delete()

user = User(name = "uName", email = "user@user.com", uid = "000000000", userType = UserTypesEnum.PROFESSOR)
user.add(user)
print ("User ID {}\n".format(user.id))

room = Room()
room.name="E003"

roomUser = RoomUser()
roomUser.user = user

room.users.append(roomUser)
room.active = True
room.add(room)
print ("Room ID {}\n".format(room.id))

schedule = Schedule()
schedule.room = room
schedule.userType = UserTypesEnum.PROFESSOR
schedule.dayOfWeek = DaysOfWeekEnum.MONDAY
schedule.beginTime =  datetime.now().hour
schedule.endTime = datetime.now().hour
schedule.lastUpdate = datetime.now()
schedule.active = True
schedule.add(schedule)
print ("Schedule ID {}\n".format(schedule.id))
