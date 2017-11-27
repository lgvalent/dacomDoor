import time
import requests
import dateutil.parser
# from apscheduler.schedulers.blocking import BlockingScheduler

from app.models import *

from config import nome_sala_rasp, last_update_fake

schema = RfidsPermitidosSchema()
schema2 = HorariosPermitidosSchema()
schema3 = EventosSchema()

API_URL = "http://localhost:5000"


def AtualizarRfids():
    last_update = (
        RfidsPermitidos.query
        .with_entities(RfidsPermitidos.last_update)
        .order_by(RfidsPermitidos.last_update.desc())
        .first()
    )

    # se bd vazio
    if not last_update:
        dados = {
            "last_update": last_update_fake,
            "sala": nome_sala_rasp
        }
    else:
        dados = {
            "last_update": str(last_update[0]),
            "sala": nome_sala_rasp
        }

    # post
    try:
        response = requests.post(API_URL + "/rasp/rfid", json=dados)
        r = response.json()

        # print('RESPOSTA ->', r, dados)

        # persiste
        for adicionar in r["novos"]:
            check_rfid = (
                RfidsPermitidos.query
                .filter(RfidsPermitidos.rfid == adicionar["rfid"])
                .first()
            )
            if not check_rfid:
                novo_rfid = RfidsPermitidos(
                    adicionar["rfid"],
                    adicionar["tipo"],
                    dateutil.parser.parse(adicionar["last_update"])
                )
                novo_rfid.add(novo_rfid)

        # remove
        for remover in r["removidos"]:
            check_rfid = (
                RfidsPermitidos.query
                .filter(RfidsPermitidos.rfid == remover["rfid"])
                .first()
            )
            if check_rfid:
                check_rfid.delete(check_rfid)

    except:
        pass


def AtualizarHorarios():
    # query
    horarios_permitidos_query = (
        HorariosPermitidos.query
        .order_by(HorariosPermitidos.last_update.desc())
        .first()
    )
    horarios_permitidos = schema2.dump(horarios_permitidos_query).data

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


def EnviarEventos():
    dados = {"sala": nome_sala_rasp}

    try:
        response = requests.post(API_URL + "/rasp/checkevento", json=dados)
        r = response.json()

        hora = dateutil.parser.parse(r["horario"])
        eventos = (
            Eventos.query
            .filter(Eventos.horario > hora)
            .all()
        )

        for e in eventos:
            adicionar = {
                "rfid": e.rfid,
                "evento": e.evento.name,
                "horario": str(e.horario),
                "sala": nome_sala_rasp
            }
            response2 = requests.post(API_URL + "/rasp/evento", json=adicionar)
            r2 = response2.json()

    except:
        pass


def tasks():
    AtualizarRfids()
    time.sleep(10)
    AtualizarHorarios()
    time.sleep(10)
    EnviarEventos()
    time.sleep(10)
    tasks()


if __name__ == '__main__':
    tasks()
    # scheduler = BlockingScheduler()
    # scheduler.add_job(AtualizarRfids, 'interval', seconds=61)
    # scheduler.add_job(AtualizarHorarios, 'interval', seconds=63)
    # scheduler.add_job(EnviarEventos, 'interval', seconds=67)
    # scheduler.start()
