from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db
from app.models.enums import Evento, EnumEvento


class Eventos(db.Model, CRUD):
	__tablename__ = "eventos"

	id = db.Column(db.Integer, primary_key=True)
	rfid = db.Column(db.String(16), nullable=False)
	evento = db.Column(db.Enum(Evento), nullable=False)
	horario = db.Column(db.DateTime, nullable=False)

	def __init__(self, rfid, evento, horario):
		self.rfid = rfid
		self.evento = evento
		self.horario = datetime.now()


class EventosSchema(Schema):
	rfid = fields.String()
	evento = EnumEvento()
	horario = fields.DateTime()
