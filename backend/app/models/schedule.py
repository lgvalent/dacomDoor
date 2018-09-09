import enum
from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db

from app.models.user import UserTypeField, UserTypesEnum

# datetime.weekday() return 0..6 from monday to synday
class DaysOfWeekEnum(enum.Enum):
	MONDAY = "Segunda-feira"
	THUESDAY = "Terça-feira"
	WEDNESDAY = "Quarta-feira"
	THURSDAY = "Quinta-feira"
	FRIDAY = "Sexta-feira"
	SATURDAY = "Sábado"
	SUNDAY = "Domingo"
	ALL = "Todos"

class DayOfWeekField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return value.name

class Schedule(db.Model, CRUD):
	__tablename__ = "schedules"

	id = db.Column(db.Integer, primary_key=True)
	roomId = db.Column(db.Integer, db.ForeignKey("rooms.id"), nullable=False)
	room = db.relationship("Room")
	userType = db.Column(db.Enum(UserTypesEnum), nullable=False)
	dayOfWeek = db.Column(db.Enum(DaysOfWeekEnum), nullable=False)
	beginTime = db.Column(db.Time, nullable=False)
	endTime = db.Column(db.Time, nullable=False)
	lastUpdate = db.Column(db.DateTime, nullable=False)
	active = db.Column(db.Boolean, nullable=False)

class ScheduleSchema(Schema):
	id = fields.Integer()
	roomId = fields.Integer()
	userType = UserTypeField()
	dayOfWeek = DayOfWeekField()
	beginTime = fields.Time()
	endTime = fields.Time()
	lastUpdate = fields.DateTime()
	active = fields.Boolean()

	class Meta:
		type_ = "schedule"
