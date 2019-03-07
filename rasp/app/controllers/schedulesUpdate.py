import requests
import dateutil.parser

from app import db

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
        db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events
        # query
        schedule = (
            Schedule.query
            .order_by(Schedule.lastUpdate.desc())
            .first()
        )

       # if DB is empty
        if not schedule:
            lastUpdate = config.ROOM_LAST_UPDATE_FAKE
        else:
            lastUpdate = schedule.lastUpdate

        # post
        try:
            response = requests.get(config.URL_SERVER + "/doorlock/" + config.ROOM_NAME + "/schedules", {"lastUpdate" : lastUpdate})
            if(response.status_code == 204): #NO CONTENT
                return

            if(response.status_code != 200):
                raise requests.exceptions.RequestException("Response: Code[{}]\n{}".format(response.status_code, response.text))

            results = response.json()

            # persiste
            for result in results["updated"]:
                schedule = Schedule.query.get(result["id"])
                if not schedule:
                    print ("Adding new schedule {}.".format(result))
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
                    print ("Updating schedule {}.".format(result))
                    schedule.dayOfWeek = result["dayOfWeek"]
                    schedule.beginTime = result["beginTime"]
                    schedule.endTime = result["endTime"]
                    schedule.userType = result["userType"]
                    schedule.lastUpdate = dateutil.parser.parse(result["lastUpdate"])
                    schedule.update()

            # remove
            for result in results["removed"]:
                print ("Removing schedule {}.".format(result))
                schedule = (
                    Schedule.query
                    .filter(schedule.id == result["id"])
                    .first()
                )
                if schedule:
                    schedule.delete(schedule)
        except requests.exceptions.RequestException as e:
            print (e)
            pass
