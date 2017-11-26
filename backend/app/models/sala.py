from marshmallow import Schema, fields

from app.models.base import CRUD, db
from app.models.horario import HorarioSchema

from datetime import datetime


class Sala(db.Model, CRUD):
	__tablename__ = "sala"

	id = db.Column(db.Integer, primary_key=True)
	nome = db.Column(db.String(20), unique=True)
	horarios = db.relationship("Horario", cascade="delete")
	acesso = db.relationship("DireitoAcesso", cascade="delete")
	last_update = db.Column(db.DateTime(), nullable=False)
	alive = db.Column(db.Boolean, nullable=False)

	def __init__(self, nome):
		self.nome = nome
		self.last_update = datetime.now()
		self.alive = True


class SalaSchema(Schema):
	id = fields.Integer()
	nome = fields.String()
	horarios = fields.Nested(HorarioSchema, many=True)
	last_update = fields.DateTime()
	alive = fields.Boolean()

	class Meta:
		type_ = "sala"
