import re
from flask_jwt import jwt_required
from flask_restful import Resource, reqparse

from app.models.sala import Sala
from app.models.acesso import DireitoAcesso
from app.models.usuario import Usuario, UsuarioSchema


user_schema = UsuarioSchema()


class SearchUsuario(Resource):
	@jwt_required()
	def post(self):
		regex = "[^\w\d]"
		parser = reqparse.RequestParser()

		parser.add_argument("query", type=str, location="json")
		parser.add_argument("filter", type=str, location="json")
		args = parser.parse_args(strict=True)

		if re.findall(regex, args["query"]):
			return None, 400

		user_query_base = Usuario.query.filter(Usuario.alive)

		if args["filter"] == "Todos":
			usuario_list = user_query_base.all()

		elif args["filter"] == "Tipo":
			usuario_list = (
				user_query_base
				.filter(Usuario.tipo.like("{}%".format(args["query"])))
				.all()
			)
		elif args["filter"] == "Nome":
			usuario_list = (
				user_query_base
				.filter(Usuario.nome.like("{}%".format(args["query"])))
				.all()
			)
		elif args["filter"] == "Sala":
			id_sala = (
				Sala.query
				.with_entities(Sala.id)
				.filter(Sala.nome == args["query"])
				.first()
			)

			if not id_sala:
				return None, 404

			acessos = (
				DireitoAcesso.query
				.filter(DireitoAcesso.alive)
				.filter(DireitoAcesso.id_sala == id_sala)
				.all()
			)
			usuario_list = []
			for acesso in acessos:
				user = (
					user_query_base
					.filter(Usuario.id == acesso.id_usuario)
					.first()
				)
				usuario_list.append(user.__dict__.copy())

		return user_schema.dump(usuario_list, many=True).data
