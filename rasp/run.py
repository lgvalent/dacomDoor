import time

from app.requisitions.atualizar_rfids import AtualizarRfid
from app.requisitions.atualizar_horarios import AtualizarHorario
from app.requisitions.atualizar_eventos import AtualizarEventos

def tasks():
    AtualizarRfid.requisitar()
    time.sleep(3)
    AtualizarHorario.requisitar()
    time.sleep(3)
    AtualizarEventos.enviar()
    time.sleep(3)
    tasks()

if __name__ == '__main__':
    tasks()
