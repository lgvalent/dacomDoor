from flask import jsonify
from flask_restful import Resource, reqparse
from sqlalchemy import desc

from app.models.sala import Sala
from app.models.evento import Evento, EventoSchema

schema = EventoSchema()


class RaspCheckEventoResource(Resource):
    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument("sala", type=str, required=True, location="json")
        args = parser.parse_args(strict=True)

        sala, results = args["sala"], {}

        sala = (
            Sala.query
            .filter(Sala.nome == sala)
            .first()
        )

        if not sala == None:
            evento = (
                Evento.query
                .filter(Evento.id_sala == sala.id)
                .order_by(desc(Evento.horario))
                .first()
            )

            if evento == None:
                results["horario"] = "2001-01-01T00:00:00+00:00"
            else:
                results["horario"] = evento.horario

        return jsonify(results)
