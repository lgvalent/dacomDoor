from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse

from app import db
from app.utils import send_message, rollback
from app.models.enums import TipoUsuario
from app.models.horario import Horario, HorarioSchema

schema = HorarioSchema()


def check_args(args):
	check = len([arg for arg in args.values() if not arg]) == 0
	if check:
		try:
			TipoUsuario(args["tipo_user"])
		except ValueError:
			check = False
	return check


def check_horario(args):
	return (
		Horario.query
		.filter(Horario.id_sala == args["id_sala"])
		.filter(Horario.dia == args["dia"])
		.filter(Horario.tipo_user == args["tipo_user"])
		.filter(Horario.hora_inicio == args["hora_inicio"])
		.filter(Horario.hora_fim == args["hora_fim"])
		.first()
	)


def get_args():
	parser = reqparse.RequestParser()
	parser.add_argument("id_sala", type=int, required=True, location="json")
	parser.add_argument("hora_inicio", type=str, required=True, location="json")
	parser.add_argument("hora_fim", type=str, required=True, location="json")
	parser.add_argument("dia", type=str, required=True, location="json")
	parser.add_argument("tipo_user", type=str, required=True, location="json")

	args = parser.parse_args(strict=True)

	return args if check_args(args) else False


class HorarioResource(Resource):
	@jwt_required()
	def get(self, id):
		horario = Horario.query.get(id)
		if not horario.alive:
			return send_message("O horário {} não existe".format(id), 404)
		return schema.dump(horario).data, 200

	@jwt_required()
	def put(self, id):
		args = get_args()

		if not args:
			return send_message("Parâmetros inválidos!", 422)

		horario = Horario.query.get(id)

		if not horario or not horario.alive:
			return send_message("O horário {} não existe".format(id), 404)

		if check_horario(args):
			return send_message("Este horário já existe", 409)

		if args["hora_inicio"] != horario.hora_inicio:
			horario.hora_inicio = args["hora_inicio"]

		if args["hora_fim"] != horario.hora_fim:
			horario.hora_fim = args["hora_fim"]

		if args["dia"] != horario.dia:
			horario.dia = args["dia"]

		if args["tipo_user"] != horario.tipo_user:
			horario.tipo_user = args["tipo_user"]

		horario.last_update = datetime.now()
		horario.update()
		return schema.dump(horario).data

	@jwt_required()
	def delete(self, id):
		horario = Horario.query.get(id)
		if not horario.alive:
			return send_message("O horário {} não existe".format(id), 404)
		horario.alive = False
		horario.last_update = datetime.now()
		horario.update()
		return None, 204


class HorarioListResource(Resource):
	# @jwt_required()
	def get(self):
		horarios = (
			Horario.query
			.filter(Horario.alive)
			.order_by(Horario.dia)
			.all()
		)
		return schema.dump(horarios, many=True).data, 200

	@jwt_required()
	def post(self):
		args = get_args()
		horario_check = check_horario(args)

		if horario_check and not horario_check.alive:
			query = Horario.query.get(horario_check.id)
			query.alive = True
			query.last_update = datetime.now()
			query.update()
			return schema.dump(query).data, 201
		elif horario_check and horario_check.alive:
			return send_message("Este horário já existe", 409)

		try:
			horario = Horario(
				args["id_sala"],
				args["dia"],
				args["hora_inicio"],
				args["hora_fim"],
				args["tipo_user"]
			)
			horario.add(horario)
			query = Horario.query.get(horario.id)
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return schema.dump(query).data, 201
