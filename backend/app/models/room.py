from marshmallow import Schema, fields

from app.models.base import CRUD, db
from app.models.schedule import Schedule, ScheduleSchema
from app.models.user import User, UserSchema

from datetime import datetime

class RoomUser(db.Model, CRUD):
	__tablename__ = "roomsUsers"

	userId = db.Column(db.Integer, db.ForeignKey('users.id'), primary_key=True)
	roomId = db.Column(db.Integer, db.ForeignKey('rooms.id'), primary_key=True)
	user = db.relationship("User")
	room = db.relationship("Room")

	lastUpdate = db.Column(db.DateTime(), nullable=False)
	active = db.Column(db.Boolean, nullable=False)

	description = db.Column(db.String(255), unique=True)

	def __init__(self):
		self.lastUpdate = datetime.now()
		self.active = True

class Room(db.Model, CRUD):
	__tablename__ = "rooms"

	id = db.Column(db.Integer, primary_key=True)
	name = db.Column(db.String(20), unique=True)
	schedules = db.relationship("Schedule", backref="room.id", cascade="delete", lazy=False)
	users = db.relationship("RoomUser", cascade="all", lazy=False)
	lastUpdate = db.Column(db.DateTime(), nullable=False)
	lastSynchronization = db.Column(db.DateTime(), nullable=True)
	lastAddress = db.Column(db.String(100), nullable=True)
	active = db.Column(db.Boolean, nullable=False)

	def __init__(self):
		self.lastUpdate = datetime.now()
		self.active = True


class RoomUserSchema(Schema):
	userId = fields.Integer()
	roomId = fields.Integer()
	user = fields.Nested(UserSchema)
	lastUpdate = fields.DateTime()
	active = fields.Boolean()
	description = fields.String()

	class Meta:
		type_ = "roomUser"

class RoomSchema(Schema):
	id = fields.Integer()
	name = fields.String()
	schedules = fields.Nested(ScheduleSchema, many=True)
	users = fields.Nested(RoomUserSchema, many=True)
	lastUpdate = fields.DateTime()
	lastSynchronization = fields.DateTime()
	lastAddress = fields.String()
	active = fields.Boolean()

	class Meta:
		type_ = "room"

