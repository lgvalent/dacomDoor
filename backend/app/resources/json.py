from flask_jwt import jwt_required
from flask_restful import Resource
from marshmallow import Schema, fields

from app.models.sala import Sala


class JSONSchema(Schema):
	id = fields.Integer()
	nome = fields.String()


class JSONResource(Resource):
	@jwt_required()
	def get(self, tipo):
		schema = JSONSchema()
		if tipo == "salas":
			salas = (
				Sala.query
				.with_entities(Sala.id, Sala.nome)
				.filter(Sala.alive)
				.order_by(Sala.nome)
				.all()
			)
			return schema.dump(salas, many=True).data, 200
