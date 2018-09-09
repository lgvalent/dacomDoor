import enum
from datetime import datetime
from app.models.base import CRUD, db
from app.models.user import UserSchema
from app.models.room import RoomSchema
from marshmallow import Schema, fields, post_load

class EventTypesEnum(enum.Enum):
	IN = "get in"
	OUT = "get ou"

class EventTypeField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return value.name

class Event(db.Model, CRUD):
	__tablename__ = "events"

	id = db.Column(db.Integer, primary_key=True)
	eventType = db.Column(db.Enum(EventTypesEnum), nullable=False)
	dateTime = db.Column(db.DateTime, nullable=False)
	userId = db.Column(db.Integer, db.ForeignKey("users.id"), nullable=False)
	user = db.relationship("User")
	roomId = db.Column(db.Integer, db.ForeignKey("rooms.id"), nullable=False)
	room = db.relationship("Room")

	def __init__(self):
		self.lastUpdate = datetime.now()
		self.active = True

class EventSchema(Schema):
	id = fields.Integer()
	eventType = EventTypeField()
	dateTime = fields.DateTime()
	userId = fields.Nested(UserSchema)
	roomId = fields.Nested(RoomSchema)

'''
	@post_load
	def fromJson(self, data):
		event = Event()
		event.roomId = data['roomId'] if 'roomId' in data else None
		event.userId = data['userId'] if 'userId' in data else None
		event.eventType = EventTypesEnum[data['eventType']]
		event.dateTime = data['dateTime']

		return event
'''