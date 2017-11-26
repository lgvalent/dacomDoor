from datetime import datetime
from marshmallow import Schema, fields

from app.models.base import CRUD, db
from app.models.acesso import AcessoSchema
from app.models.enums import TipoUsuario, EnumTipo


class Usuario(db.Model, CRUD):
	__tablename__ = "usuario"

	id = db.Column(db.Integer, primary_key=True)
	nome = db.Column(db.String(80), nullable=False)
	tipo = db.Column(db.Enum(TipoUsuario), nullable=False)
	email = db.Column(db.String(100), nullable=False, unique=True)
	rfid = db.Column(db.String(16), nullable=False, unique=True)
	direito_acesso = db.relationship("DireitoAcesso", cascade="delete")
	last_update = db.Column(db.DateTime(), nullable=False)
	alive = db.Column(db.Boolean, nullable=False)

	def __init__(self, nome, email, rfid, tipo):
		self.nome = nome
		self.email = email
		self.rfid = rfid
		self.tipo = tipo
		self.alive = True
		self.last_update = datetime.now()


class UsuarioSchema(Schema):
	id = fields.Integer()
	nome = fields.String()
	email = fields.Email()
	rfid = fields.String()
	tipo = EnumTipo()
	last_update = fields.DateTime()
	direito_acesso = fields.Nested(AcessoSchema, many=True)
	alive = fields.Boolean()

	class Meta:
		type_ = "usuario"
