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
    try:
        results = (
            Event.query
            .all()
        )

        response = requests.post(config.URL_SERVER + "/doorlock/" + config.ROOM_NAME + "/events", json=EventSchema().dump(events, many=True).data)

        if(not response.ok):
            print ("Error updating events on server")
        else:
            Event.query.delete()

    except:
        pass