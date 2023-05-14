package br.edu.utfpr.cm.dacom.dacomdoor.services;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.exception.MessageList;
import br.com.orionsoft.monstrengo.core.service.ServiceBasic;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.core.service.ServiceException;
import br.com.orionsoft.monstrengo.crud.entity.EntityList;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.services.UtilsCrud;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.Room;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.RoomUser;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.User;

/**
 * Serviço que garante ou revoga acesso dos usuários às salas
 * 
 * <p>
 * <b>Argumento:</b><br>
 * 
 * <p>
 * <b>Procedimento:</b>
 * 
 * @author Lucio 
 * @version 20200322
 */
public class RoomsUsersService extends ServiceBasic {
	public static final String SERVICE_NAME = "RoomsUsersService";

	/*
	 * Parâmetros de entrada do serviço.
	 */
	public static final String IN_ROOMS = "rooms";
	public static final String IN_USERS = "users";
	public static final String IN_DATE = "date";
	public static final String IN_OPERATION = "operation";
	public static final String IN_DESCRIPTION_OPT = "description";

	public enum Operation {
		GRANT,
		REVOKE;
	}    

	public String getServiceName() {
		return SERVICE_NAME;
	}

	@SuppressWarnings("unchecked")
	public void execute(ServiceData serviceData) throws ServiceException {
		try {	

			EntityList<User> inUsers = (EntityList<User>) serviceData.getArgumentList().getProperty(IN_USERS);
			EntityList<Room> inRooms = (EntityList<Room>) serviceData.getArgumentList().getProperty(IN_ROOMS);
			Operation inOperation = (Operation) serviceData.getArgumentList().getProperty(IN_OPERATION);
			Calendar inDate = (Calendar) serviceData.getArgumentList().getProperty(IN_DATE);
			String inDescription = (serviceData.getArgumentList().containsProperty(IN_DESCRIPTION_OPT) ? 
					(String) serviceData.getArgumentList().getProperty(IN_DESCRIPTION_OPT) : null);

			if(log.isDebugEnabled())log.debug("Processing " + inUsers.getSize() + " users and " + inRooms.getSize() + " rooms.");
			this.addInfoMessage(serviceData, "INFO", inUsers.getSize(), inRooms.getSize());

			for(IEntity<User> user: inUsers)
				if(user.isSelected())
					for(IEntity<Room> room: inRooms)
						if(room.isSelected()) {
							User oUser = user.getObject();
							Room oRoom = room.getObject();

							if(log.isDebugEnabled())log.debug("Processing user " + user + " and room " + room + ".");
							boolean found = false;
							for(RoomUser roomUser: oUser.getRoomsUser()) {
								if(roomUser.getRoom().getId() == room.getId()) {
									/* REVOKE */
									if(roomUser.isActive() && inOperation == Operation.REVOKE) {
										roomUser.setActive(false); 
										roomUser.setLastUpdate(inDate);
										this.addInfoMessage(serviceData, "REVOKE_UPDATE", user.toString(), room.toString());
									} 

									/* GRANT inactive */
									if(!roomUser.isActive() && inOperation == Operation.GRANT) {
										roomUser.setActive(true); 
										roomUser.setLastUpdate(inDate);
										roomUser.setDescription(roomUser.getDescription() + "\n" + inDescription);
										this.addInfoMessage(serviceData, "GRANT_UPDATE", user.toString(), room.toString());
									} 

									found = true;
									break;
								}
							}

							/* GRANT adding new record */
							if(!found && inOperation == Operation.GRANT) {
								RoomUser roomUser_ = new RoomUser();
								roomUser_.setRoom(room.getObject());
								roomUser_.setUser(user.getObject());
								roomUser_.setDescription(inDescription);
								roomUser_.setActive(true);
								roomUser_.setLastUpdate(inDate);

								oUser.getRoomsUser().add(roomUser_);

								this.addInfoMessage(serviceData, "GRANT", user.toString(), room.toString());
							}

							UtilsCrud.update(getServiceManager(), user, serviceData);

						}



			//			serviceData.getMessageList().add();

		} catch (Exception e) {
			throw new ServiceException(MessageList.createSingleInternalError(e));
		}
	}
}