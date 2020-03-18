package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.exception.BusinessException;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoBasic;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoException;
import br.com.orionsoft.monstrengo.security.entities.UserSession;

public class RoomDvo extends DvoBasic<Room> {

	public Class<Room> getEntityType() {
		return Room.class;
	}
	
	public void afterUpdate(IEntity<Room> entity, UserSession userSession, ServiceData serviceData) throws DvoException, BusinessException {
		Room oRoom = entity.getObject();
				
		/* Mark the register as modified to be updated by peers*/
		oRoom.setLastUpdate(Calendar.getInstance());
	}

	@Override
	public void afterCreate(IEntity<Room> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void afterDelete(IEntity<Room> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeCreate(IEntity<Room> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeDelete(IEntity<Room> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeUpdate(IEntity<Room> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}
}