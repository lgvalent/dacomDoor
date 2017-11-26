from marshmallow import Schema, fields

from app.models.base import db, CRUD
from app.models.enums import TipoUsuario, EnumTipo


class RfidsPermitidos(db.Model, CRUD):
	__tablename__ = "rfids_permitidos"

	id = db.Column(db.Integer, primary_key=True)
	rfid = db.Column(db.String(16), nullable=False, unique=True)
	tipo = db.Column(db.Enum(TipoUsuario), nullable=False)
	last_update = db.Column(db.DateTime(), nullable=False)

	def __init__(self, rfid, tipo, last_update):
		self.rfid = rfid
		self.tipo = tipo
		self.last_update = last_update


class RfidsPermitidosSchema(Schema):
	rfid = fields.String()
	tipo = EnumTipo()
	last_update = fields.DateTime()
