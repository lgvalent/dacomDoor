package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.exception.BusinessException;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoBasic;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoException;
import br.com.orionsoft.monstrengo.security.entities.UserSession;

public class RoomUserDvo extends DvoBasic<RoomUser> {
	
	public Class<RoomUser> getEntityType() {
		return RoomUser.class;
	}
	
	public void afterUpdate(IEntity<RoomUser> entity, UserSession userSession, ServiceData serviceData) throws DvoException, BusinessException {
		RoomUser oRoomUser = entity.getObject();
				
		/* Mark the register as modified to be updated by peers*/
		oRoomUser.setLastUpdate(Calendar.getInstance());
	}

	@Override
	public void afterCreate(IEntity<RoomUser> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void afterDelete(IEntity<RoomUser> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeCreate(IEntity<RoomUser> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeDelete(IEntity<RoomUser> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeUpdate(IEntity<RoomUser> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}
}