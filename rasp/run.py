import time
import argparse

from app.requisitions.atualizar_rfids import AtualizarRfid
from app.requisitions.atualizar_horarios import AtualizarHorario
from app.requisitions.atualizar_eventos import AtualizarEventos

parser = argparse.ArgumentParser(description='digite run.py apenas ou acrescente [-s "nomesala"] para salvar o nome da sala')
parser.add_argument('-s', dest='nome_sala', help='exemplo: "python3 run.py -s E101"')
args = parser.parse_args()

if args.nome_sala != None:
    try:
        arquivo = open('config.py', 'r+')
        sala = arquivo.readline()
        if "nome_sala_rasp" not in sala:
            arquivo.seek(0)
        restante = arquivo.read()
        arquivo.seek(0)
        arquivo.write('nome_sala_rasp = "' + args.nome_sala + '"\n' + restante)
        arquivo.close()
    except:
        pass

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
