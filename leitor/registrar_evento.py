from sqlalchemy.orm import sessionmaker

from conexaoBD import *

def salvar(rfid, hora, tipo_evento):
    session = iniciar_sessao()
    
    novo_evento = Eventos(rfid=rfid, horario=hora, tipo=tipo_evento)
    session.add(novo_evento)
    session.commit()
    session.close()