from app import api
from app.resources import *

#api.add_resource(SearchUsuario, "/pesquisa")
#api.add_resource(SearchEvento,"/pesquisa/evento")

#api.add_resource(AdminResource, "/admins/<int:id>")
#api.add_resource(AdminListResource, "/admins")

api.add_resource(UserResource, "/user/<int:id>")
api.add_resource(UserListResource, "/users")

api.add_resource(RoomResource, "/room/<int:id>")
api.add_resource(RoomListResource, "/rooms")
api.add_resource(RoomUserListResource, "/room/<int:roomId>/users", "/room/<int:roomId>/users/<int:userId>")

api.add_resource(DoorlockKeyringsResource, "/doorlock/<string:roomName>/keyrings")
api.add_resource(DoorlockEventsResource, "/doorlock/<string:roomName>/events")
api.add_resource(DoorlockSchedulesResource, "/doorlock/<string:roomName>/schedules")
#api.add_resource(SalaHorarioResource, "/salas/horarios/<int:id>")#

api.add_resource(ScheduleResource, "/schedule/<int:id>")
api.add_resource(ScheduleListResource, "/schedules")

#api.add_resource(JSONResource, "/json/<string:tipo>")

#api.add_resource(RaspRfidResource, "/query/keyrings")
#api.add_resource(RaspHorarioResource, "/rasp/horario")
#api.add_resource(RaspCheckEventoResource, "/rasp/checkevento")
#api.add_resource(RaspEventoResource, "/rasp/evento")
