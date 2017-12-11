from autenticador import gerenciar
from abrir_porta import abrir
from registrar_evento import salvar
import datetime

def checkar_credenciais(uid, tipo_evento):
    permitido = gerenciar(uid, tipo_evento)
    if permitido:
        abrir()
        salvar(rfid, datetime.now(), tipo_evento)
    else:
        print('Rfid: %s nao permitido', uid)