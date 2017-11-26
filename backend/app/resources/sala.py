from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse

from app import db
from app.models.sala import Sala, SalaSchema
from app.models.horario import Horario
from app.models.acesso import DireitoAcesso
from app.utils import send_message, rollback

schema = SalaSchema()


def check_args():
	parser = reqparse.RequestParser()
	parser.add_argument("nome", type=str, required=True, location="json")
	args = parser.parse_args(strict=True)

	return args if args["nome"] else None


class SalaHorarioResource(Resource):
	@jwt_required()
	def delete(self, id):
		try:
			query = Horario.query.filter(Horario.id_sala == id)
			horarios = [i for i in query.all()]

			for horario in horarios:
				horario.alive = False
				horario.last_update = datetime.now()
				horario.update()
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return None, 204


class SalaResource(Resource):
	@jwt_required()
	def get(self, id):
		sala = Sala.query.get(id)
		if not sala.alive:
			return send_message("A sala {} não existe".format(id))
		return schema.dump(sala).data

	@jwt_required()
	def put(self, id):
		args = check_args()
		if not args:
			return send_message("O nome da sala não pode ser vazio", 422)

		sala = Sala.query.get(id)

		if not sala.alive:
			return send_message("A sala {} não existe".format(id))

		if sala.nome != args["nome"]:
			sala.nome = args["nome"]

		try:
			sala.last_update = datetime.now()
			sala.update()
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return schema.dump(sala).data

	@jwt_required()
	def delete(self, id):
		try:
			sala = Sala.query.get(id)
			if not sala or not sala.alive:
				return send_message("A sala {} não existe".format(id))
			sala.alive = False

			horarios = (
				Horario.query
				.filter(Horario.id_sala == id)
				.all()
			)

			acessos = (
				DireitoAcesso.query
				.filter(DireitoAcesso.id_sala == id)
				.all()
			)

			for acesso in acessos:
				acesso.alive = False

			for horario in horarios:
				horario.alive = False

			sala.last_update = datetime.now()
			sala.update()
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return None, 204


class SalaListResource(Resource):
	# @jwt_required()
	def get(self):
		query = Sala.query.filter(Sala.alive)
		salas = [sala.__dict__ for sala in query.all()]

		for sala in salas:
			sala["horarios"] = (
				Horario.query
				.filter(Horario.alive)
				.filter(Horario.id_sala == sala["id"])
				.order_by(Horario.dia)
				.all()
			)
		return schema.dump(salas, many=True).data, 200

	# @jwt_required()
	def post(self):
		args = check_args()

		if not args:
			return send_message("O nome da sala não pode ser vazio", 422)

		sala = (
			Sala.query
			.filter(Sala.alive == False)
			.filter(Sala.nome == args["nome"])
			.first()
		)

		if sala:
			sala.alive = True
			sala.last_update = datetime.now()
			sala.update()
			return schema.dump(sala).data, 201
		else:
			try:
				sala = Sala(args["nome"])
				sala.add(sala)
				query = Sala.query.order_by(Sala.id.desc()).first()
			except SQLAlchemyError as e:
				return rollback(e, db)
			else:
				return schema.dump(query).data, 201
