from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse
from flask import request

from app import db
from app.models.user import User, UserSchema
from app.models.room import Room, RoomSchema, RoomUser, RoomUserSchema
from app.models.schedule import Schedule, ScheduleSchema
from app.utils import response, rollback

schema = RoomSchema()
roomUserSchema = RoomUserSchema()

def check_args():
	parser = reqparse.RequestParser()
	parser.add_argument("name", type=str, required=True, location="json")
	args = parser.parse_args(strict=True)

	return args if args["name"] else None

class RoomResource(Resource):
	#@jwt_required()
	def get(self, id):
		room = Room.query.get(id)
		if not room.active:
			return response("Room {} not exists".format(id))
		return schema.dump(room).data, 200

	#@jwt_required()
	def put(self, id):
		args = check_args()
		if not args:
			return response("Room name required", 422)

		room = Room.query.get(id)

		if not room.active:
			return response("Room {} not exists".format(id))

		if room.name != args["name"]:
			room.name = args["name"]

		try:
			room.lastUpdate = datetime.now()
			room.update()
		except SQLAlchemyError as e:
			return rollback(e, db), 406
		else:
			return schema.dump(room).data, 202

	#@jwt_required()
	def delete(self, id):
		try:
			room = Room.query.get(id)
			if not room or not room.active:
				return response("Room {} not exists".format(id))
			room.active = False

			room.lastUpdate = datetime.now()
			room.update()
		except SQLAlchemyError as e:
			return rollback(e, db), 406
		else:
			return None, 202


class RoomListResource(Resource):
	# @jwt_required()
	def get(self):
		query = Room.query.filter(Room.active)

		if request.args.get("name"): query = query.filter(User.email.like("%" + request.args["name"] + "%"))

		if request.args.get("orderBy"): query = query.order_by(request.args["orderBy"])

		rooms = [room.__dict__ for room in query.all()]

		'''
		for room in rooms:
			room["schedules"] = (
				Schedule.query
				.filter(Schedule.active)
				.filter(Schedule.roomId == room["id"])
				.order_by(Schedule.dayOfWeek)
				.all()
			)
			room["users"] = (
				RoomUser.query
				.filter(RoomUser.active)
				.filter(RoomUser.roomId == room["id"])
				.order_by(RoomUser.userId)
				.all()
			)
		'''
		return schema.dump(rooms, many=True).data, 200

	# @jwt_required()
	def post(self):
		args = check_args()

		if not args:
			return response("Room's name is required", 422)

		room = (
			Room.query
			.filter(Room.active == False)
			.filter(Room.name == args["name"])
			.first()
		)

		if room:
			room.active = True
			room.lastUpdate = datetime.now()
			room.update()
			return schema.dump(room).data, 201
		else:
			try:
				room = Room()
				room.name = args["name"]
				room.add(room)
			except SQLAlchemyError as e:
				return rollback(e, db), 406
			else:
				return schema.dump(room).data, 201


class RoomUserListResource(Resource):
	# @jwt_required()
	def get(self, roomId, userId=None):
		query = RoomUser.query.filter(RoomUser.roomId == roomId)
		if userId: query = query.filter(RoomUser.userId == userId)
		
		roomUsers = query.all()
		
		if len(roomUsers) <= 0:
			return "There is no Users to Room {}".format(roomId), 404
		
		return roomUserSchema.dump(roomUsers, many=True).data, 200

	# @jwt_required()
	def post(self, roomId, userId):
		roomUser = (RoomUser.query
				.filter(RoomUser.roomId == roomId)
				.filter(RoomUser.userId == userId)
				.first()
		)

		if roomUser:
			roomUser.active = True
			roomUser.lastUpdate = datetime.now()
			roomUser.update()
			return roomUserSchema.dump(roomUser).data, 201
		else:
			try:
				roomUser = RoomUser()
				roomUser.roomId = roomId
				roomUser.userId = userId
				roomUser.add(roomUser)
			except SQLAlchemyError as e:
				return rollback(e, db), 406
			else:
				return roomUserSchema.dump(roomUser).data, 201

	# @jwt_required()
	def delete(self, roomId, userId):
		roomUser = (RoomUser.query
				.filter(RoomUser.active)
				.filter(RoomUser.roomId == roomId)
				.filter(RoomUser.userId == userId)
				.first()
		)

		if roomUser:
			try:
				roomUser.active = False
				roomUser.lastUpdate = datetime.now()
				roomUser.update()
			except SQLAlchemyError as e:
				return rollback(e, db), 406
			else:
				return "User {} removed from Room {}".format(userId, roomId), 202
			
		return "User {} was not found in Room {}".format(userId, roomId), 404

