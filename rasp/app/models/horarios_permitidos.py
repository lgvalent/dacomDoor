from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import db, CRUD
from app.models.enums import Dia, TipoUsuario, EnumDia, EnumTipo


class HorariosPermitidos(db.Model, CRUD):
	__tablename__ = "horarios_permitidos"

	id = db.Column(db.Integer, primary_key=True)
	dia = db.Column(db.Enum(Dia), nullable=False)
	hora_inicio = db.Column(db.Time, nullable=False)
	hora_fim = db.Column(db.Time, nullable=False)
	tipo_usuario = db.Column(db.Enum(TipoUsuario), nullable=False)
	last_update = db.Column(db.DateTime(), nullable=False)

	def __init__(self, id, dia, hora_inicio, hora_fim, tipo_usuario, last_update):
		self.id = id
		self.dia = dia
		self.hora_inicio = hora_inicio
		self.hora_fim = hora_fim
		self.tipo_usuario = tipo_usuario
		self.last_update = last_update


class HorariosPermitidosSchema(Schema):
	id = fields.Integer()
	dia = EnumDia()
	hora_inicio = fields.Time()
	hora_fim = fields.Time()
	tipo_usuario = EnumTipo()
	last_update = fields.DateTime()
