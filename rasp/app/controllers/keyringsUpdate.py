import requests
import dateutil.parser
from app import db
from datetime import datetime

from app.models.keyrings import Keyring, KeyringSchema

import config

class KeyringsUpdate:
    '''
    - rasp busca a ultima data de um rfid cadastrado ou atualizado com o servidor
    - rasp envia pro servidor o nome da sala desse rasp e a data do passo anterior (data coringa se nao houver registros)
    - servidor confere se existem novos rfids cadastrados ou modificados para essa sala depois da data em questao
    - servidor envia de volta uma lista de novos rfids, rfids modificados e rfids removidos
    - rasp insere novos rfids, modifica rfids existentes e remove rfids, se necessario
    '''

    def update(self):
        db.session.commit() #Lucio 20180912: This session was not synchronyzing with DB when other process inserts events
        keyring = (
            Keyring.query
            .with_entities(Keyring.lastUpdate)
            .order_by(Keyring.lastUpdate.desc())
            .first()
        )

        # if DB is empty
        if not keyring:
            lastUpdate = config.ROOM_LAST_UPDATE_FAKE
        else:
            lastUpdate = keyring.lastUpdate

        # post
        try:
            response = requests.get(config.URL_SERVER + "/doorlock/" + config.ROOM_NAME + "/keyrings", {"lastUpdate" : lastUpdate})
            if(response.status_code == 204): #NO CONTENT
                return

            if(response.status_code != 200):
                raise requests.exceptions.RequestException("Response: Code[{}]\n{}".format(response.status_code, response.text))

            results = response.json()

            print(results)

            # remove
            for result in results["removed"]:
                keyring = (
                    Keyring.query
                    .filter(Keyring.userId == result["userId"])
                    .first()
                )
                if keyring:
                    print ("Removing local UID {}.".format(result["userId"]))
                    keyring.delete(keyring)

            # persist
            for result in results["updated"]:
                keyring = (
                    Keyring.query
                    .filter(Keyring.userId == result["userId"])
                    .first()
                )
                if not keyring:
                    print ("Adding new local UID {}.".format(result["uid"]))
                    keyring = Keyring(
                        result["uid"],
                        result["userId"],
                        result["userType"],
                        dateutil.parser.parse(result["lastUpdate"])
                    )
                    keyring.add(keyring)
                else:
                    print ("Updating local UID {}.".format(result["uid"]))
                    keyring.uid = result["uid"]
                    keyring.userType = result["userType"]
                    keyring.lastUpdate = dateutil.parser.parse(result["lastUpdate"])
                    keyring.update()

        except requests.exceptions.RequestException as e:
            print (e)
            pass
