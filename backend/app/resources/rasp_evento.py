import dateutil.parser
from flask_restful import Resource, reqparse
from sqlalchemy.exc import SQLAlchemyError

from app import db
from app.models.sala import Sala
from app.models.usuario import Usuario
from app.models.evento import Evento, EventoSchema
from app.utils import rollback

schema = EventoSchema()


class RaspEventoResource(Resource):
    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument("rfid", type=str, required=True, location="json")
        parser.add_argument("evento", type=str, required=True, location="json")
        parser.add_argument("horario", type=str, required=True, location="json")
        parser.add_argument("sala", type=str, required=True, location="json")
        args = parser.parse_args(strict=True)

        sala = (
            Sala.query
            .filter(Sala.nome == args["sala"])
            .first()
        )

        usuario = (
            Usuario.query
            .filter(Usuario.rfid == args["rfid"])
            .first()
        )

        try:
            evento = Evento(
                args["evento"],
                dateutil.parser.parse(args["horario"]),
                usuario.id,
                sala.id
            )
            evento.add(evento)
            query = Evento.query.get(evento.id)
        except SQLAlchemyError as e:
            return rollback(e, db)
        else:
            return schema.dump(query).data, 201
