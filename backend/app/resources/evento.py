import re

from datetime import datetime
from flask_jwt import jwt_required
from sqlalchemy.exc import SQLAlchemyError
from flask_restful import Resource, reqparse
from sqlalchemy import and_

import dateutil.parser

from app.models.evento import Evento, EventoSchema
from app.utils import send_message, rollback

from app.models.usuario import Usuario
from app.models.sala import Sala

schema = EventoSchema()



class SearchEvento(Resource):
	
	def normalize_event(self,eventos):
		list_eventos=[]

		for evento in eventos:
			new_evento = evento.__dict__.copy()

			relatorio = []
			usuario = Usuario.query.get(new_evento.id_usuario)
			sala = Sala.query.get(new_evento.id_sala)
			evento = new_evento.evento
			data = "{}/{}/{}".format(new_evento.data.day,new_evento.data.month,new_evento.data.year)
			hora = "{}:{}".format(new_evento.data.hour,new_evento.data.minute)
			relatorio= {
				"usuario": usuario.nome,
				"sala": sala.nome,
				"evento": evento,
				"data": data,
				"hora":hora
			}
			list_eventos.append(relatorio)
		return list_eventos

	
	@jwt_required()
	def post(self):
		regex = "[^\w\d]"
		parser = reqparse.RequestParser()

		parser.add_argument("query", type=str, location="json")
		parser.add_argument("filter", type=str, location="json")
		args = parser.parse_args(strict=True)

		if re.findall(regex, args["query"]):
			return None, 400

		if args["filter"] == "Usuario":
			if not args["query"]:
				   return None, 404
			id_user = (
					Usuario.query
					.with_entities(Usuario.id)
					.filter(Usuario.nome == args["query"])
					.first()

			)

			if not id_user:
				return None, 404

			eventos = (
					Evento.query
					.filter(Evento.id_usuario == args["query"])
					.all()
			)
			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos,many = True).data

		if args["filter"] == "Data":
			if not args["query"]:
				return None, 404

			
			data_inicio = args["query"]["data_inicio"]
			data_fim = args["query"]["data_fim"]
			eventos = (
				Evento.query
				.filter(
					and_( Evento.horario > data_inicio, Evento.horario < data_fim )
				)
				.all()
			)

			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos,many = True).data

		if args["filter"] == "Tipo":
			if not args["query"]:
				return None, 404

			eventos = (
				Evento.query.filter(Evento.evento == args["query"]).all()
			)

			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos,many = True).data

		if args["filter"] == "Sala":
			if not args["query"]:
				return None, 404

			sala_id = Sala.query(Sala.id).filter(Sala.nome == args["query"])
			eventos = (
				Evento.query.filter(Evento.id_sala == sala_id).all()
			)

			list_eventos = self.normalize_event(eventos)
			return schema.dump(list_eventos,many = True).data       