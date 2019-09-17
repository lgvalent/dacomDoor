from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from sqlalchemy import or_
from flask_restful import Resource, reqparse
from flask import request
from flask import jsonify

from app import db
from app.models.event import Event, EventSchema, EventTypesEnum
from app.models.user import User, UserSchema, UserTypesEnum
from app.models.room import Room, RoomSchema, RoomUser, RoomUserSchema
from app.models.schedule import Schedule, ScheduleSchema
from app.utils import response, rollback

'''
   URL Example: http://localhost:5000/doorlock/E003/keyrings?lastUpdate=2018-07-01 00:00:00
'''
class DoorlockKeyringsResource(Resource):
	def get(self, roomName):
		try:
			room = Room.query.filter(Room.name == roomName).first()
			room.lastSynchronization = datetime.now()
			room.lastAddress = request.remote_addr
			room.update()
		except SQLAlchemyError as e:
			print(e)

		lastUpdate = datetime.strptime(request.args.get("lastUpdate"), "%Y-%m-%d %H:%M:%S")

		results, updated, removed = {}, [], []

		roomUsers = RoomUser.query.join(RoomUser.user).join(RoomUser.room).filter(Room.name == roomName).filter(or_(RoomUser.lastUpdate > lastUpdate, User.lastUpdate > lastUpdate)).all()

		if len(roomUsers) == 0:
			return "", 204
		for roomUser in roomUsers:
			roomId = roomUser.roomId
			if roomUser.active:
				updated.append({
					"userId" : roomUser.userId,
					"uid" : roomUser.user.uid,
					"userType" : roomUser.user.userType.name,
					"lastUpdate" : roomUser.lastUpdate if roomUser.lastUpdate > roomUser.user.lastUpdate else roomUser.user.lastUpdate
				})
			else:
				removed.append({"userId": roomUser.userId})
		results["updated"] = updated
		results["removed"] = removed

		return jsonify(results)

'''
  Request Example:
    Path:   http://localhost:5000/doorlock/E003/events
    Method: PUT
    Content-Type:   application/json
    Content: [{"uid":"111111", "eventType":"IN", "dateTime": "2007-07-07 12:12:12"},
              {"uid":"111112", "eventType":"IN", "dateTime": "2007-07-07 13:13:13"}]
'''
class DoorlockEventsResource(Resource):
	def post(self, roomName):
		room = Room.query.filter(Room.name == roomName).first()

		if not room:
			return "Room {} not found.".format(roomName), 404

		events = request.get_json()
		eventSchema = EventSchema()

		Event.begin()
		for evt in events:
			user = User.query.filter(User.uid == evt['uid']).first()
			if(user == None):
				#Lucio 20190516: Add unknown users
				User.begin()
				user = User("UNKNOWN USER", "unknown@user.com", evt['uid'], UserTypesEnum.STUDENT)
				user.add(user)
				User.commit()

			result = eventSchema.load(evt)
			if(len(result.errors)>0):
				return 'Error parsing content:{}.'.format(result.errors), 400

			event = Event()
			event.roomId = room.id
			event.userId = user.id
			event.eventType = EventTypesEnum[result.data['eventType']]
			event.dateTime = result.data['dateTime']
			event.add(event)
		Event.commit()

		return "All events updated", 200

class DoorlockSchedulesResource(Resource):
	def get(self, roomName):
		lastUpdate = datetime.strptime(request.args.get("lastUpdate"), "%Y-%m-%d %H:%M:%S")

		results, updated, removed = {}, [], []

		schedules = Schedule.query.join(Schedule.room).filter(Room.name == roomName).filter(Schedule.lastUpdate > lastUpdate).all()

		if len(schedules) == 0:
			return "", 204

		for schedule in schedules:
			if schedule.active:
				updated.append({
					"id" : schedule.id,
					"roomId" : schedule.roomId,
					"userType" : schedule.userType.name,
					"dayOfWeek" : schedule.dayOfWeek.name,
					"beginTime" : schedule.beginTime.strftime("%H:%M"),
					"endTime" : schedule.endTime.strftime("%H:%M"),
					"lastUpdate" : schedule.lastUpdate
				})
			else:
				removed.append({"id": schedule.id})
		results["updated"] = updated
		results["removed"] = removed
		return jsonify(results)
