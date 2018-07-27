import requests
import dateutil.parser

from app.models.events import Event, EventSchema

import config

class EventsUpdate:
    '''
    - rasp envia pro servidor o nome da sala desse rasp
    - servidor retorna a data do ultimo evento recebido (data coringa se nao houver registros)
    - rasp confere se existem novos eventos registrados depois da data em questao
    - rasp envia pro servidor todos os novos registros encontrados
    - servidor insere novos eventos
    '''
    
    def update(self):
        data = {"room": config.ROOM_NAME}

    try:
        response = requests.post(config.URL_SERVER+ "/rasp/checkLastEvent", json=data)
        results = response.json()

        lastUpdate = dateutil.parser.parse(results["lastUpdate"])
        results = (
            Event.query
            .filter(Event.time > lastUpdate)
            .all()
        )

        events = {
                "room": config.ROOM_NAME,
                "events": []
        }

        for result in results:
            event = {
                "uid": result.uid,
                "event": result.event.name,
                "time": str(result.time),
            }
            events["events"].append(event)
        
        response = requests.post(config.URL_SERVER + "/rasp/events", json=events)
        if(not response.ok):
            print ("Error updating events on server")

    except:
        pass