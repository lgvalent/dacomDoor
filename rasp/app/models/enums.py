import enum
from marshmallow import fields

class DayOfWeekField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return DaysOfWeekEnum[value].name

# datetime.weekday() return 0..6 from monday to synday
class DaysOfWeekEnum(enum.Enum):
	MONDAY = "Segunda-feira"
	THUESDAY = "Terça-feira"
	WEDNESDAY = "Quarta-feira"
	THURSDAY = "Quinta-feira"
	FRIDAY = "Sexta-feira"
	SATURDAY = "Sábado"
	SUNDAY = "Domingo"


class UserTypeField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return UserTypesEnum[value].name

class UserTypesEnum(enum.Enum):
	STUDENT = "Student"
	PROFESSOR = "Professor"
	EMPLOYEE = "Employee"

class EventTypeField(fields.Field):
	def _serialize(self, value, attr, obj):
		if value is None:
			return ""
		else:
			return EventTypesEnum[value].name


class EventTypesEnum(enum.Enum):
	IN = "get in"
	OUT = "get out"
