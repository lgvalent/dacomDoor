from sqlalchemy import create_engine, Column, Integer, String, DateTime, ForeignKey, Time
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
import datetime

Base = declarative_base()
# tipo_evento = "entrada"

class RfidsPermitidos(Base):
    __tablename__ = "rfids_permitidos"

    id = Column('id', Integer, primary_key=True)
    rfid = Column('rfid', String)
    tipo = Column('tipo', String)

class HorariosPermitidos(Base):
    __tablename__ = "horarios_permitidos"

    id = Column('id', Integer, primary_key=True)
    dia = Column('dia', String)
    tipo_usuario = Column('tipo_usuario', String)
    hora_inicio = Column('hora_inicio', Time)
    hora_fim = Column('hora_fim', Time)

class Eventos(Base):
    __tablename__ = "eventos"

    id = Column('id', Integer, primary_key=True)
    rfid = Column('rfid', String)
    tipo = Column('evento', String)
    horario = Column('horario', DateTime)

def dia_da_semana(dia):
    if dia == 0:
        return "Segunda"
    if dia == 1:
        return "Terca"
    if dia == 2:
        return "Quarta"
    if dia == 3:
        return "Quinta"
    if dia == 4:
        return "Sexta"
    if dia == 5:
        return "Sabado"
    if dia == 6:
        return "Domingo"

def abrir_porta():
    pass

def registrar_evento(rfid, hora, session):
    novo_evento = Eventos(rfid=rfid, horario=hora, tipo="entrada") #tipo=tipo_evento)
    session.add(novo_evento)
    session.commit()

def autorizar(rfid):
    bd_id = "root1"
    bd_pw = "root"
    bd_end = "localhost"
    bd_name = "rasp"

    engine = create_engine('mysql+pymysql://'+bd_id+':'+bd_pw+'@'+bd_end+'/'+bd_name)
    Session = sessionmaker(bind=engine)
    session = Session()

    rfids_permitidos = session.query(RfidsPermitidos).filter(RfidsPermitidos.rfid == rfid).first()
    if rfids_permitidos != None:                                                    # se o rfid existe na tabela de rfids_permitidos
        agora = datetime.datetime.now()
        dia = dia_da_semana(agora.weekday())
        hora = datetime.time(agora.hour, agora.minute, agora.second)
        if(rfids_permitidos.tipo != "Aluno"):                                       # se não for do tipo "aluno", pode entrar qualquer um
            abrir_porta()
            registrar_evento(rfid, agora, session)
        else:
            horarios_permitidos = (session.query(HorariosPermitidos)
                .filter(HorariosPermitidos.tipo_usuario == rfids_permitidos.tipo)   # se é do tipo "Aluno"
                .filter(HorariosPermitidos.dia == dia)                              # se é do mesmo dia
                .filter(HorariosPermitidos.hora_inicio <= hora)                     # se a hora for depois da permitida
                .filter(HorariosPermitidos.hora_fim >= hora)                        # e a hora for antes do limite permitido
                .all()
            )
            if horarios_permitidos != []:                                           # se houve algum resultado, existe um horário permitido, deixa entrar
                abrir_porta()
                registrar_evento(rfid, agora, session)

    session.close()