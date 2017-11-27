from flask import jsonify
from flask_restful import Resource, reqparse

from app.models.usuario import Usuario


class RaspRfidResource(Resource):
    def post(self):
        parser = reqparse.RequestParser()
        parser.add_argument("last_update", type=str, required=True, location="json")
        parser.add_argument("sala", type=str, required=True, location="json")
        args = parser.parse_args(strict=True)

        results, results_novos, results_removidos = {}, [], []
        usuarios = (
            Usuario.query
            .filter(Usuario.last_update > args["last_update"])
            .filter(Usuario.alive)
            .all()
        )

        for u in usuarios:
            for d in u.direito_acesso:
                if (d.nome_sala.nome == args["sala"]):
                    if d.alive:
                        results_novos.append({
                            "rfid": u.rfid,
                            "tipo": u.tipo.name,
                            "last_update": u.last_update
                        })
                    else:
                        results_removidos.append({"rfid": u.rfid})

        results["novos"] = results_novos.copy()

        usuarios = (
            Usuario.query
            .filter(Usuario.last_update > args["last_update"])
            .filter(Usuario.alive == False)
            .all()
        )
        
        for u in usuarios:
            for d in u.direito_acesso:
                if (d.nome_sala.nome == args["sala"]):
                    results_removidos.append({"rfid": u.rfid})
        results["removidos"] = results_removidos.copy()

        return jsonify(results)
