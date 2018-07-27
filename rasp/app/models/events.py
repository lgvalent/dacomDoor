from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db
from app.models.enums import EventTypesEnum, EventTypeField


class Event(db.Model, CRUD):
	__tablename__ = "events"

	id = db.Column(db.Integer, primary_key=True)
	uid = db.Column(db.String(16), nullable=False)
	eventType = db.Column(db.Enum(EventTypesEnum), nullable=False)
	time = db.Column(db.DateTime, nullable=False)

	def __init__(self, uid, eventType, time):
		self.uid = uid
		self.eventType = eventType
		self.time = time

class EventSchema(Schema):
	uid = fields.String()
	eventType = EventTypeField()
	time = fields.DateTime()
