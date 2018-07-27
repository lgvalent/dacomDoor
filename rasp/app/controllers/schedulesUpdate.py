import requests
import dateutil.parser

from app.models.schedules import Schedule, ScheduleSchema

import config

class SchedulesUpdate:
    '''
    - rasp busca a ultima data de um horario cadastrado ou atualizado
    - rasp envia pro servidor o nome da sala desse rasp e a data do passo anterior (data coringa se nao houver registros)
    - servidor confere se existem novos horarios cadastrados ou modificados para essa sala depois da data em questao
    - servidor envia de volta uma lista de novos horarios, horarios modificados e horarios removidos
    - rasp insere novos horarios, modifica horarios existentes e remove horarios, se necessario
    '''
    
    def update(self):
        # query
        schedule = (
            Schedule.query
            .order_by(Schedule.lastUpdate.desc())
            .first()
        )

        # se bd vazio
        if not schedule:
            data = {
                "lastUpdate": config.ROOM_LAST_UPDATE_FAKE,
                "room": config.ROOM_NAME
            }
        else:
            data = {
                "lastUpdate": schedule.lastUpdate,
                "room": config.ROOM_NAME
            }

        # post
        try:
            response = requests.post(config.URL_SERVER + "/rasp/schedule", json=data)
            results = response.json()

            # persiste
            for result in results["updated"]:
                schedule = Schedule.query.get(result["id"])
                if not schedule:
                    schedule = Schedule(
                        result["id"],
                        result["dayOfWeek"],
                        result["beginTime"],
                        result["endTime"],
                        result["userType"],
                        dateutil.parser.parse(result["lastUpdate"])
                    )
                    schedule.add(schedule)
                else:
                    schedule.dayOfWeek = result["dayOfWeek"]
                    schedule.beginTime = result["beginTime"]
                    schedule.endTime = result["endTime"]
                    schedule.userType = result["userType"]
                    schedule.lastUpdate = dateutil.parser.parse(result["lastUpdate"])
                    schedule.update()

            # remove
            for result in results["removed"]:
                schedule = (
                    Schedule.query
                    .filter(schedule.id == result["id"])
                    .first()
                )
                if schedule:
                    schedule.delete(schedule)
        except:
            pass