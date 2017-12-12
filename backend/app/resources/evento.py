from flask_jwt import jwt_required
from flask_restful import Resource, reqparse

from app.models.evento import Evento, SearchEventoSchema

from app.models.usuario import Usuario
from app.models.sala import Sala

schema = SearchEventoSchema()


class SearchEvento(Resource):
	def normalize_event(self, eventos):
		list_eventos = []

		for evento in eventos:
			new_evento = evento.__dict__.copy()

			print("CAGADA ->", new_evento.keys())

			relatorio = []
			usuario = Usuario.query.get(new_evento["id_usuario"])
			sala = Sala.query.get(new_evento["id_sala"])
			evento = new_evento["evento"]
			data = "{}/{}/{}".format(
				new_evento["horario"].day,
				new_evento["horario"].month,
				new_evento["horario"].year
			)
			hora = "{}:{}".format(
				new_evento["horario"].hour,
				new_evento["horario"].minute
			)
			relatorio = {
				"usuario": usuario.nome,
				"sala": sala.nome,
				"evento": evento,
				"data": data,
				"hora": hora
			}
			list_eventos.append(relatorio)
		return list_eventos

	@jwt_required()
	def post(self):
		parser = reqparse.RequestParser()

		parser.add_argument("query", type=dict, location="json")
		parser.add_argument("filter", type=str, location="json")
		args = parser.parse_args(strict=True)

		if args["filter"] == "Usuário":
			if not args["query"]["usuario"]:
				return None, 404
			id_user = (
				Usuario.query
				.with_entities(Usuario.id)
				.filter(Usuario.nome.like("{}%".format(args["query"]["usuario"])))
				.first()
			)

			if not id_user:
				return None, 404

			eventos = (
				Evento.query
				.filter(Evento.id_usuario == id_user[0])
				.all()
			)
			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos, many=True).data

		if args["filter"] == "Data":
			if not args["query"]:
				return None, 404
			data_inicio = args["query"]["data_inicio"]
			data_fim = args["query"]["data_fim"]
			eventos = (
				Evento.query
				.filter(Evento.horario > data_inicio)
				.filter(Evento.horario < data_fim)
				.all()
			)

			list_eventos = self.normalize_event(eventos)
			if not len(list_eventos):
				return None, 404
			return schema.dump(list_eventos, many=True).data

		if args["filter"] == "Tipo":
			if not args["query"]["tipo"]:
				return None, 404

			eventos = (
				Evento.query
					.filter(Evento.evento == args["query"]["tipo"])
					.all()
			)

			list_eventos = self.normalize_event(eventos)
			if not len(list_eventos):
				return None, 404
			return schema.dump(list_eventos, many=True).data

		if args["filter"] == "Sala":
			if not args["query"]["sala"]:
				return None, 404

			sala_id = (
				Sala.query
					.with_entities(Sala.id)
					.filter(Sala.nome == args["query"]["sala"])
					.first()
			)

			if not sala_id:
				return None, 404

			eventos = (
				Evento.query
					.filter(Evento.id_sala == sala_id[0])
					.all()
			)

			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos, many=True).data
