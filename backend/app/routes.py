from app import api
from app.resources import *


api.add_resource(SearchUsuario, "/pesquisa")

api.add_resource(AdminResource, "/admins/<int:id>")
api.add_resource(AdminListResource, "/admins")

api.add_resource(UsuarioResource, "/usuarios/<int:id>")
api.add_resource(UsuarioListResource, "/usuarios")

api.add_resource(SalaResource, "/salas/<int:id>")
api.add_resource(SalaListResource, "/salas")
api.add_resource(SalaHorarioResource, "/salas/horarios/<int:id>")

api.add_resource(HorarioResource, "/horarios/<int:id>")
api.add_resource(HorarioListResource, "/horarios")

api.add_resource(JSONResource, "/json/<string:tipo>")

api.add_resource(RaspRfidResource, "/rasp/rfid")
api.add_resource(RaspHorarioResource, "/rasp/horario")
api.add_resource(RaspCheckEventoResource, "/rasp/checkevento")
api.add_resource(RaspEventoResource, "/rasp/evento")
