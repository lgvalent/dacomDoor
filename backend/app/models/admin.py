from marshmallow import Schema, fields
from passlib.apps import custom_app_context as pwd_context

from app.models.base import CRUD, db


class Admin(db.Model, CRUD):
	__tablename__ = "admins"

	id = db.Column(db.Integer, primary_key=True)
	name = db.Column(db.String(80), nullable=False)
	email = db.Column(db.String(100), nullable=False, unique=True)
	password = db.Column(db.String(128), nullable=False)

	def __init__(self, name, email, password):
		self.name = name
		self.email = email
		self.password = pwd_context.encrypt(password)

	def hash_password(self, password):
		self.password = pwd_context.encrypt(password)

	def verify_password(self, password):
		return pwd_context.verify(password, self.password)


class AdminSchema(Schema):
	id = fields.Integer()
	name = fields.String()
	email = fields.Email()
	password = fields.String()

	class Meta:
		type_ = "admin"
