import enum
from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import db, CRUD
from app.models.keyrings import UserTypesEnum, UserTypeField

# datetime.weekday() return 0..6 from monday to synday
class DaysOfWeekEnum(enum.Enum):
	MONDAY = "Segunda-feira"
	THUESDAY = "Terça-feira"
	WEDNESDAY = "Quarta-feira"
	THURSDAY = "Quinta-feira"
	FRIDAY = "Sexta-feira"
	SATURDAY = "Sábado"
	SUNDAY = "Domingo"

class DayOfWeekField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return value.name

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
