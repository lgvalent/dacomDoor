from sqlalchemy import create_engine, Column, Integer, String, DateTime, ForeignKey, Time
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

from config import *

Base = declarative_base()

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

def iniciar_sessao():
    engine = create_engine('mysql+pymysql://'+bd_id+':'+bd_pw+'@'+bd_end+'/'+bd_name)
    Session = sessionmaker(bind=engine)
    session = Session()
    return session
