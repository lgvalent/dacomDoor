import requests
import dateutil.parser

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
            results = response.json()

            # persist
            for result in results["updated"]:
                keyring = (
                    Keyring.query
                    .filter(Keyring.userId == result["userId"])
                    .first()
                )
                if not keyring:
                    keyring = Keyring(
                        result["uid"],
                        result["userId"],
                        result["userType"],
                        dateutil.parser.parse(result["lastUpdate"])
                    )
                    keyring.add(keyring)
                else:
                    keyring.uid = result["uid"]
                    keyring.userType = result["userType"]
                    keyring.lastUpdate = dateutil.parser.parse(result["lastUpdate"])
                    keyring.update()

            # remove
            for result in results["removed"]:
                keyring = (
                    Keyring.query
                    .filter(Keyring.userId == result["userId"])
                    .first()
                )
                if keyring:
                    keyring.delete(keyring)

        except:
            pass
