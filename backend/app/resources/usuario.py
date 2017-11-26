import re
from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse

from app import db
from app.models.acesso import DireitoAcesso
from app.models.usuario import Usuario, UsuarioSchema
from app.utils import send_message, rollback
from app.models.enums import TipoUsuario

schema = UsuarioSchema()


def check_args(args):
	_args = args.copy()
	_args.pop("direito_acesso")  # removi pois pode ser vazio
	check = all(_args.values())  # verifica se os campos não estão vazios
	if check:
		email_re = r"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)"
		check = False if not re.findall(email_re, args["email"]) else True
	return check


def get_args():
	parser = reqparse.RequestParser()

	parser.add_argument("nome", type=str, location="json")
	parser.add_argument("rfid", type=str, location="json")
	parser.add_argument("tipo", type=str, location="json")
	parser.add_argument("email", type=str, location="json")
	parser.add_argument("direito_acesso", type=dict, action="append", location="json")
	args = parser.parse_args(strict=True)

	return args if check_args(args) else False


def get_acessos(id):
	return (
		DireitoAcesso.query
		.filter(DireitoAcesso.alive == True)
		.filter(DireitoAcesso.id_usuario == id)
		.all()
	)


def gerencia_acessos(user, direito_acesso=None):
	new_user = user.__dict__.copy()
	if not direito_acesso:
		for acesso in user.direito_acesso:
			acesso.alive = False
	else:
		args = [i["id_sala"] for i in direito_acesso]

		# id do usuário
		acessos = [acesso.id_sala for acesso in user.direito_acesso]

		# está no JSON, mas não no BANCO
		to_add = [i for i in args if i not in acessos]

		for acesso in user.direito_acesso:
			# sala está no JSON e BANCO, mas está 'morto' no banco
			if acesso.id_sala in args and not acesso.alive:
				acesso.alive = True
			# sala não está no JSON e está no BANCO como 'vivo'
			elif acesso.id_sala not in args and acesso.alive:
				acesso.alive = False

		for new_acesso in to_add:
			acesso = DireitoAcesso(user.id, new_acesso)
			acesso.add(acesso)

	user.last_update = datetime.now()
	new_user["tipo"] = TipoUsuario(user.tipo)
	new_user["direito_acesso"] = get_acessos(user.id)

	return new_user


class UsuarioResource(Resource):
	@jwt_required()
	def get(self, id):
		user = Usuario.query.get(id)

		if not user.alive:
			return send_message("O usuário {} não existe".format(id))

		user = user.__dict__.copy()
		user["direito_acesso"] = get_acessos(id)
		return schema.dump(user).data, 200

	@jwt_required()
	def put(self, id):
		args = get_args()

		if not args:
			return send_message("Parâmetros inválidos", 422)

		user = Usuario.query.get(id)

		if not user.alive:
			return send_message("O usuário {} não existe".format(id))

		if user.rfid != args["rfid"]:
			rfid_query = Usuario.query.filter(Usuario.rfid == args["rfid"])
			if rfid_query.all():
				return send_message("RFID existente", 409)
			else:
				user.rfid = args["rfid"]

		if user.nome != args["nome"]:
			user.nome = args["nome"]

		if user.email != args["email"]:
			email_query = Usuario.query.filter(Usuario.email == args["email"])
			if email_query.all():
				return send_message("Email existente", 409)
			else:
				user.email = args["email"]

		if user.tipo != args["tipo"]:
			user.tipo = args["tipo"]

		new_user = gerencia_acessos(user, args["direito_acesso"])
		user.update()

		return schema.dump(new_user).data

	@jwt_required()
	def delete(self, id):
		try:
			user = Usuario.query.get(id)
			if not user.alive:
				return send_message("O usuário {} não existe".format(id))
			user.alive = False
			for acesso in user.direito_acesso:
				acesso.alive = False
			user.last_update = datetime.now()
			user.update()
		except SQLAlchemyError as e:
			return rollback(e, db)
		else:
			return None, 204


class UsuarioListResource(Resource):
	#@jwt_required()
	def get(self):
		usuarios = Usuario.query.filter(Usuario.alive).all()
		usuarios = [i.__dict__ for i in usuarios]

		for usuario in usuarios:
			usuario["direito_acesso"] = get_acessos(usuario["id"])

		results = schema.dump(usuarios, many=True).data
		return results

	@jwt_required()
	def post(self):
		args = get_args()

		if not args:
			return send_message("Parâmetros inválidos", 422)

		user = (
			Usuario.query
			.filter(Usuario.alive == False)
			.filter(Usuario.email == args["email"])
			.first()
		)

		if user:
			return self.resurrect(user, args)
		else:
			try:
				user = Usuario(args["nome"], args["email"], args["rfid"], args["tipo"])
				user.add(user)
				if args["direito_acesso"]:
					try:
						for sala in args["direito_acesso"]:
							acesso = DireitoAcesso(user.id, sala["id_sala"])
							acesso.add(acesso)
					except SQLAlchemyError as e:
						return rollback(e, db)
				query = Usuario.query.get(user.id)
			except SQLAlchemyError as e:
				return rollback(e, db)
			else:
				return schema.dump(query).data, 201

	def resurrect(self, user, args):
		user.alive = True
		user.nome = args["nome"]
		user.tipo = args["tipo"]

		if user.rfid != args["rfid"]:
			if Usuario.query.filter(Usuario.rfid == args["rfid"]).all():
				return send_message("RFID existente", 409)
			else:
				user.rfid = args["rfid"]

		new_user = gerencia_acessos(user, args["direito_acesso"])
		user.update()

		return schema.dump(new_user).data, 201
