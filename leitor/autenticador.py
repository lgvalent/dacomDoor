from sqlalchemy.orm import sessionmaker
import datetime

from conexaoBD import *

def gerenciar(rfid, tipo_evento):
    session = iniciar_sessao()

    rfids_permitidos = session.query(RfidsPermitidos).filter(RfidsPermitidos.rfid == rfid).first()
    if rfids_permitidos != None:                                               
        agora = datetime.datetime.now()
        dia = dia_da_semana(agora.weekday())
        hora = datetime.time(agora.hour, agora.minute, agora.second)
        if(rfids_permitidos.tipo != "Aluno"):                                       
            return True
        else:
            horarios_permitidos = (session.query(HorariosPermitidos)
                .filter(HorariosPermitidos.tipo_usuario == rfids_permitidos.tipo)   
                .filter(HorariosPermitidos.dia == dia)                              
                .filter(HorariosPermitidos.hora_inicio <= hora)                    
                .filter(HorariosPermitidos.hora_fim >= hora)                        
                .all()
            )
            if horarios_permitidos != []:
                return True
            else:
                return False

    session.close()