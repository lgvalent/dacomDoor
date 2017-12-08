import requests
import dateutil.parser

from app.models.horarios_permitidos import HorariosPermitidos, HorariosPermitidosSchema

from config import nome_sala_rasp, last_update_fake, API_URL

schema = HorariosPermitidosSchema()

class AtualizarHorario:
    
    def requisitar():
        # query
        horarios_permitidos_query = (
            HorariosPermitidos.query
            .order_by(HorariosPermitidos.last_update.desc())
            .first()
        )
        horarios_permitidos = schema.dump(horarios_permitidos_query).data

        # se bd vazio
        if not horarios_permitidos:
            dados = {
                "last_update": last_update_fake,
                "sala": nome_sala_rasp
            }
        else:
            dados = {
                "last_update": horarios_permitidos["last_update"],
                "sala": nome_sala_rasp
            }

        # post
        try:
            response = requests.post(API_URL + "/rasp/horario", json=dados)
            r = response.json()

            # persiste
            for adicionar in r["novos"]:
                horario = HorariosPermitidos.query.get(adicionar["id"])
                if not horario:
                    novo_horario = HorariosPermitidos(
                        adicionar["id"],
                        adicionar["dia"],
                        adicionar["hora_inicio"],
                        adicionar["hora_fim"],
                        adicionar["tipo_usuario"],
                        dateutil.parser.parse(adicionar["last_update"])
                    )
                    novo_horario.add(novo_horario)
                else:
                    horario.dia = adicionar["dia"]
                    horario.hora_inicio = adicionar["hora_inicio"]
                    horario.hora_fim = adicionar["hora_fim"]
                    horario.tipo_usuario = adicionar["tipo_usuario"]
                    horario.last_update = dateutil.parser.parse(adicionar["last_update"])
                    horario.update()

            # remove
            for remover in r["removidos"]:
                horarios_permitidos = (
                    HorariosPermitidos.query
                    .filter(HorariosPermitidos.id == remover["id"])
                    .first()
                )
                if horarios_permitidos:
                    horarios_permitidos.delete(horarios_permitidos)
        except:
            pass