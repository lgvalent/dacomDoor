import dateutil.parser
from flask import jsonify
from flask_restful import Resource, reqparse

from app.models.sala import Sala
from app.models.horario import Horario


class RaspHorarioResource(Resource):
    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument("last_update", type=str, required=True, location="json")
        parser.add_argument("sala", type=str, required=True, location="json")
        args = parser.parse_args(strict=True)

        sala = args["sala"]
        hora = dateutil.parser.parse(args["last_update"])
        results, horarios_novos, horarios_removidos = {}, [], []

        sala = (
            Sala.query
            .filter(Sala.nome == sala)
            .first()
        )

        if not sala == None:
            horarios_novos = (
                Horario.query
                .filter(Horario.last_update > hora)
                .filter(Horario.alive)
                .filter(Horario.id_sala == sala.id)
                .all()
            )

            horarios_removidos = (
                Horario.query
                .filter(Horario.last_update > hora)
                .filter(Horario.alive == False)
                .filter(Horario.id_sala == sala.id)
                .all()
            )

            results["novos"] = [{
                "id": h.id,
                "dia": h.dia.name,
                "hora_inicio": str(h.hora_inicio),
                "hora_fim": str(h.hora_fim),
                "tipo_usuario": h.tipo_user.name,
                "last_update": str(h.last_update)
            } for h in horarios_novos]

            results["removidos"] = [{"id": h.id} for h in horarios_removidos]

        return jsonify(results)
