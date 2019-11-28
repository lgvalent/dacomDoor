import enum
from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db

class EventTypesEnum(enum.Enum):
	IN = "get in"
	OUT = "get out"
	DOOR_OPENED = "door opened"

class EventTypeField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return value.name

class Event(db.Model, CRUD):
	__tablename__ = "events"

	id = db.Column(db.Integer, primary_key=True)
	uid = db.Column(db.String(16), nullable=False)
	eventType = db.Column(db.Enum(EventTypesEnum), nullable=False)
	dateTime = db.Column(db.DateTime, nullable=False)

	def __init__(self, uid, eventType, dateTime):
		self.uid = uid
		self.eventType = eventType
		self.dateTime = dateTime

class EventSchema(Schema):
	uid = fields.String()
	eventType = EventTypeField()
	dateTime = fields.DateTime()
