import requests
import dateutil.parser

from app import db

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
            db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events
            events = (Event.query.all())

            if len(events) > 0:
                print ("Synchronyzing {} event(s).".format(len(events)))
                response = requests.post(config.URL_SERVER + "/doorlock/" + config.ROOM_NAME + "/events", json=EventSchema().dump(events, many=True).data)
                if(response.status_code != 200):
                    raise requests.exceptions.RequestException("Response: Code[{}]\n{}".format(response.status_code, response.text))

                if(not response.ok):
                    raise requests.exceptions.RequestException("Error updating events on server:{}".format(response.msg))
                else:
                    for event in events:
                        event.delete(event)


        except requests.exceptions.RequestException as e:
            print (e)
            pass
