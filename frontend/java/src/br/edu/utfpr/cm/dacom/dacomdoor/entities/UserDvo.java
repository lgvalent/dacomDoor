package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.exception.BusinessException;
import br.com.orionsoft.monstrengo.core.exception.MessageList;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.IEntityList;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoBasic;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoException;
import br.com.orionsoft.monstrengo.crud.services.UtilsCrud;
import br.com.orionsoft.monstrengo.security.entities.UserSession;

public class UserDvo extends DvoBasic<User> {
	
	public Class<User> getEntityType() {
		return User.class;
	}

	public void afterUpdate(IEntity<User> entity, UserSession userSession, ServiceData serviceData) throws DvoException, BusinessException {
		User oUser = entity.getObject();
		
		IEntityList<User> users = UtilsCrud.list(this.getDvoManager().getEntityManager().getServiceManager(), User.class, "uid='"+oUser.getUid()+"'", null);
		if(users.size()>0 && users.getFirst().getId() != oUser.getId())
			throw new DvoException(MessageList.create(UserDvo.class, "UID_REPEATED", oUser.getDecUid(), users.getFirst()));
		
		if(entity.getProperty(User.UID).getValue().isValueNull()) {
			oUser.setActive(false);
		}
				
		/* Mark the register as modified to be updated by peers*/
		oUser.setLastUpdate(Calendar.getInstance());
		
		for(RoomUser oRoomUser: oUser.getRoomsUser()){
			for(RoomUser oRoomUserAux: oUser.getRoomsUser()){
				if(oRoomUser.getRoom().getName().equals(oRoomUserAux.getRoom().getName()) && oRoomUser.getId() != oRoomUserAux.getId()){
					throw new DvoException(MessageList.create(UserDvo.class, "ROOM_REPEATED", oRoomUser.getRoom().getName()));
				}
			}
			/* Inactivate all rooms from a inactive user */
			if(!oUser.isActive()) {
				oRoomUser.setActive(false);
				oRoomUser.setLastUpdate(Calendar.getInstance());
			}
		}
	}

	@Override
	public void afterCreate(IEntity<User> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void afterDelete(IEntity<User> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeCreate(IEntity<User> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeDelete(IEntity<User> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeUpdate(IEntity<User> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

}