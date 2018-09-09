import enum
from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db

class UserTypesEnum(enum.Enum):
	STUDENT = "Student"
	PROFESSOR = "Professor"
	EMPLOYEE = "Employee"

class User(db.Model, CRUD):
	__tablename__ = "users"

	id = db.Column(db.Integer, primary_key=True)
	name = db.Column(db.String(80), nullable=False)
	email = db.Column(db.String(100), nullable=False, unique=True)
	uid = db.Column(db.String(16), nullable=False, unique=True)
	userType = db.Column(db.Enum(UserTypesEnum), nullable=False)
	lastUpdate = db.Column(db.DateTime(), nullable=False)
	active = db.Column(db.Boolean, nullable=False)

	def __init__(self, name, email, uid, userType):
		self.name = name
		self.email = email
		self.uid = uid
		self.userType = userType
		self.active = True
		self.lastUpdate = datetime.now()

class UserTypeField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return value.name

class UserSchema(Schema):
	id = fields.Integer()
	name = fields.String()
	email = fields.Email()
	uid = fields.String()
	userType = UserTypeField()
	lastUpdate = fields.DateTime()
	active = fields.Boolean()

	class Meta:
		type_ = "user"
