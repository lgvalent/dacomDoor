from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import db, CRUD
from app.models.enums import DaysOfWeekEnum, DayOfWeekField, UserTypesEnum, UserTypeField


class Schedule(db.Model, CRUD):
	__tablename__ = "schedules"

	id = db.Column(db.Integer, primary_key=True)
	dayOfWeek = db.Column(db.Enum(DaysOfWeekEnum), nullable=False)
	beginTime = db.Column(db.Time, nullable=False)
	endTime = db.Column(db.Time, nullable=False)
	userType = db.Column(db.Enum(UserTypesEnum), nullable=False)
	lastUpdate = db.Column(db.DateTime(), nullable=False)

	def __init__(self, id, dayOfWeek, beginTime, endTime, userType, lastUpdate):
		self.id = id
		self.dayOfWeek = dayOfWeek
		self.beginTime = beginTime
		self.endTime = endTime
		self.userType = userType
		self.lastUpdate = lastUpdate

class ScheduleSchema(Schema):
	id = fields.Integer()
	dayOfWeek = DayOfWeekField()
	beginTime = fields.Time()
	endTime = fields.Time()
	userType = UserTypeField()
	lastUpdate = fields.DateTime()
