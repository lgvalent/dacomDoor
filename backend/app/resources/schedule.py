from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse

from app import db
from app.utils import response, rollback
from app.models.user import UserTypesEnum
from app.models.schedule import Schedule, ScheduleSchema, DaysOfWeekEnum

schema = ScheduleSchema()


def check_args(args):
	check = len([arg for arg in args.values() if not arg]) == 0
	if check:
		try:
			UserTypesEnum(args["userType"])
			DaysOfWeekEnum(args["dayOfWeek"])
		except ValueError:
			check = False
	return check


def check_schedule(args):
	return (
		Schedule.query
		.filter(Schedule.roomId == args["roomId"])
		.filter(Schedule.dayOfWeek == args["dayOfWeek"])
		.filter(Schedule.userType == args["userType"])
		.filter(Schedule.beginTime == args["beginTime"])
		.filter(Schedule.active == True)
		.first()
	)


def get_args():
	parser = reqparse.RequestParser()
	parser.add_argument("roomId", type=int, required=True, location="json")
	parser.add_argument("beginTime", type=str, required=True, location="json")
	parser.add_argument("endTime", type=str, required=True, location="json")
	parser.add_argument("dayOfWeek", type=str, required=True, location="json")
	parser.add_argument("userType", type=str, required=True, location="json")

	args = parser.parse_args(strict=True)

	return args if check_args(args) else False

def fill_schedule(schedule, args):
	if args["beginTime"] != schedule.beginTime:
		schedule.beginTime = args["beginTime"]

	if args["endTime"] != schedule.endTime:
		schedule.endTime = args["endTime"]

	if args["dayOfWeek"] != schedule.dayOfWeek:
		schedule.dayOfWeek = args["dayOfWeek"]

	if args["userType"] != schedule.userType:
		schedule.userType = args["userType"]

class ScheduleResource(Resource):
	#@jwt_required()
	def get(self, id):
		schedule = Schedule.query.get(id)
		if not schedule.active:
			return response("Schedule {} not exists".format(id), 404)
		return schema.dump(schedule).data, 200

	#@jwt_required()
	def put(self, id):
		args = get_args()

		if not args:
			return response("Invalid parameters", 422)

		schedule = Schedule.query.get(id)

		if not schedule or not schedule.active:
			return response("Schedule {} not exists".format(id), 404)

		if check_schedule(args):
			return response("There is a conflict in schedule", 409)

		fill_schedule(schedule, args)
		schedule.lastUpdate = datetime.now()
		schedule.update()
		return schema.dump(schedule).data, 202

	#@jwt_required()
	def delete(self, id):
		schedule = Schedule.query.get(id)
		if not schedule.active:
			return response("Schedule {} not existis".format(id), 404)
		schedule.active = False
		schedule.lastUpdate = datetime.now()
		schedule.update()
		return None, 204


class ScheduleListResource(Resource):
	#@jwt_required()
	def get(self):
		schedules = (
			Schedule.query
			.filter(Schedule.active)
			.order_by(Schedule.roomId)
			.all()
		)
		return schema.dump(schedules, many=True).data, 200

	#@jwt_required()
	def post(self):
		args = get_args()
		
		if check_schedule(args):
			return response("There is a conflict in schedule", 409)
		
		schedule = Schedule()
		fill_schedule(schedule, args)
		schedule.active = True
		schedule.lastUpdate = datetime.now()
		schedule.update()
		return schema.dump(schedule).data, 201
