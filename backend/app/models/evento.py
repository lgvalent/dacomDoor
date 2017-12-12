from app.models.base import CRUD, db
from app.models.enums import Evento, EnumEvento
from marshmallow import Schema, fields


class Evento(db.Model, CRUD):
	__tablename__ = "eventos"

	id = db.Column(db.Integer, primary_key=True)
	evento = db.Column(db.Enum(Evento), nullable=False)
	horario = db.Column(db.DateTime, nullable=False)
	id_usuario = db.Column(db.Integer, db.ForeignKey("usuario.id"), nullable=False)
	id_sala = db.Column(db.Integer, db.ForeignKey("sala.id"), nullable=False)

	def __init__(self, evento, horario, id_usuario, id_sala):
		self.evento = evento
		self.horario = horario
		self.id_usuario = id_usuario
		self.id_sala = id_sala


class EventoSchema(Schema):
	id = fields.Integer()
	evento = EnumEvento()
	horario = fields.DateTime()
	id_usuario = fields.Integer()
	id_sala = fields.Integer()


class SearchEventoSchema(Schema):
	evento = EnumEvento()
	data = fields.String()
	hora = fields.String()
	usuario = fields.String()
	sala = fields.String()
