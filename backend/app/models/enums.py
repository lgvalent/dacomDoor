import enum
from marshmallow import fields


class EnumDia(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		if value == Dia.Segunda:
			return "Segunda-feira"
		elif value == Dia.Terca:
			return "Terça-feira"
		elif value == Dia.Quarta:
			return "Quarta-feira"
		elif value == Dia.Quinta:
			return "Quinta-feira"
		elif value == Dia.Sexta:
			return "Sexta-feira"
		elif value == Dia.Sabado:
			return "Sábado"
		elif value == Dia.Domingo:
			return "Domingo"


class Dia(enum.Enum):
	Segunda = "Segunda-feira"
	Terca = "Terça-feira"
	Quarta = "Quarta-feira"
	Quinta = "Quinta-feira"
	Sexta = "Sexta-feira"
	Sabado = "Sábado"
	Domingo = "Domingo"


class EnumTipo(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		if value == TipoUsuario.Aluno:
			return "Aluno"
		elif value == TipoUsuario.Professor:
			return "Professor"
		elif value == TipoUsuario.Servidor:
			return "Servidor"


class TipoUsuario(enum.Enum):
	Aluno = "Aluno"
	Professor = "Professor"
	Servidor = "Servidor"


class EnumEvento(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		if value == Evento.entrada:
			return "Entrada"
		elif value == Evento.saida:
			return "Saída"


class Evento(enum.Enum):
	entrada = 0
	saida = 1
