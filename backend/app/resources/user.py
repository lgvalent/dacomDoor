import re
from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse
from flask import request

from app import db
from app.models.user import User, UserSchema, UserTypesEnum
from app.models.room import Room, RoomUser
from app.utils import response, rollback

schema = UserSchema()

def check_args(args):
	_args = args.copy()
	check = all(_args.values())  # verifica se os campos não estão vazios
	if check:
		email_re = r"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)"
		check = False if not re.findall(email_re, args["email"]) else True
	return check

def get_args():
	parser = reqparse.RequestParser()

	parser.add_argument("name", type=str, location="json", required=True)
	parser.add_argument("uid", type=str, location="json", required=True)
	parser.add_argument("userType", type=str, location="json", required=True)
	parser.add_argument("email", type=str, location="json", required=True)
	args = parser.parse_args(strict=True)

	return args if check_args(args) else False


class UserResource(Resource):
	#@jwt_required()
	def get(self, id):
		user = User.query.get(id)

		if not user.active:
			return response("User {} not found".format(id))

		user = user.__dict__.copy()
		return schema.dump(user).data, 200

	#@jwt_required()
	def put(self, id):
		args = get_args()

		if not args:
			return response("Invalid parameters", 422)

		user = User.query.get(id)

		if not user.active:
			return response("User {} not exists".format(id))

		if user.uid != args["uid"]:
			rfid_query = User.query.filter(User.uid == args["uid"])
			if rfid_query.all():
				return response("UID already exists", 409)
			else:
				user.rfid = args["uid"]

		if user.name != args["name"]:
			user.name = args["name"]

		if user.email != args["email"]:
			email_query = User.query.filter(User.email == args["email"])
			if email_query.all():
				return response("Email already exists", 409)
			else:
				user.email = args["email"]

		if user.userType != args["userType"]:
			user.userType = args["userType"]

		user.update()

		return schema.dump(user).data

	#@jwt_required()
	def delete(self, id):
		try:
			user = User.query.get(id)
			if not user.active:
				return response("User {} not exists".format(id))

			user.active = False
			user.lastUpdate = datetime.now()
			user.update()
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return None, 204


class UserListResource(Resource):
	#@jwt_required()
	def get(self):
		query = User.query.filter(User.active)

		if request.args.get("email"): query = query.filter(User.email.like("%" + request.args["email"] + "%"))
		if request.args.get("name"): query = query.filter(User.name.like("%" + request.args["name"] + "%"))
		if request.args.get("uid"): query = query.filter(User.uid.like("%" + request.args["uid"] + "%"))
		if request.args.get("userType"): query = query.filter(User.userType == request.args["userType"])
		if request.args.get("roomId"): query = query.join(RoomUser.user).filter(RoomUser.roomId == request.args["roomId"])

		if request.args.get("orderBy"): query = query.order_by(request.args["orderBy"])

		users = query.all()
		users = [i.__dict__ for i in users]

		if len(users) == 0 :
			return response("No record found", 204)

		return schema.dump(users, many=True).data, 200

	#@jwt_required()
	def post(self):
		args = get_args()

		if not args:
			return response("Invalid parameters", 422)

		user = (
			User.query
			.filter(User.active == False)
			.filter(User.email == args["email"])
			.first()
		)

		if user:
			return self.reactive(user, args)
		else:
			try:
				user = User(name=args["name"], email=args["email"], uid=args["uid"], userType=args["userType"])
				user.add(user)
			except SQLAlchemyError as e:
				return rollback(e, db), 406
			else:
				return schema.dump(user).data, 201

	def reactive(self, user, args):
		user.active = True
		user.lastUpdate = datetime.now()
		user.name = args["name"]
		user.userType = args["userType"]

		if user.uid != args["uid"]:
			if User.query.filter(user.uid == args["uid"]).all():
				return response("UID {} already in use".format(user.uid), 409)
			else:
				user.uid = args["uid"]

		user.update()

		return schema.dump(user).data, 201
