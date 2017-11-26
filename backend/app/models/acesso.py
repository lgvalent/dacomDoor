from marshmallow import Schema, fields

from app.models.base import db, CRUD


class DireitoAcesso(db.Model, CRUD):
	__tablename__ = "direito_acesso"

	id = db.Column(db.Integer, primary_key=True)
	id_usuario = db.Column(db.Integer, db.ForeignKey("usuario.id"), nullable=False)
	id_sala = db.Column(db.Integer, db.ForeignKey("sala.id"), nullable=False)
	nome_sala = db.relationship("Sala", uselist=False)
	alive = db.Column(db.Boolean, nullable=False)

	def __init__(self, id_usuario, id_sala):
		self.id_usuario = id_usuario
		self.id_sala = id_sala
		self.alive = True


# Schema p/ nome_sala
class CustomSchema(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		return value.nome


class AcessoSchema(Schema):
	id_sala = fields.Integer()
	nome_sala = CustomSchema()
	alive = fields.Boolean()

	class Meta:
		type_ = "acesso"


class AcessoSchemaRasp(Schema):
	id_sala = fields.Integer()
	nome_sala = CustomSchema()
	id_usuario = fields.Integer()
	alive = fields.Boolean()

	class Meta:
		type_ = "acessorasp"
