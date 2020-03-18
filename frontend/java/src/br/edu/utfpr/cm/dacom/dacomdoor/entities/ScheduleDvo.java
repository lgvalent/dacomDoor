package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.exception.BusinessException;
import br.com.orionsoft.monstrengo.core.exception.MessageList;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoBasic;
import br.com.orionsoft.monstrengo.crud.entity.dvo.DvoException;
import br.com.orionsoft.monstrengo.security.entities.UserSession;

public class ScheduleDvo extends DvoBasic<Schedule> {
	
	public Class<Schedule> getEntityType() {
		return Schedule.class;
	}
	
	public void afterUpdate(IEntity<Schedule> entity, UserSession userSession, ServiceData serviceData) throws DvoException, BusinessException {
		
		Schedule oSchedule = entity.getObject();

		if(oSchedule.getBeginTime().getTime() > oSchedule.getEndTime().getTime())
			throw new DvoException(MessageList.create(ScheduleDvo.class, "BEGIN_END_DATE_INCOMPATIBLE"));
				
		/* Mark the register as modified to be updated by peers*/
		oSchedule.setLastUpdate(Calendar.getInstance());
	}

	@Override
	public void afterCreate(IEntity<Schedule> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void afterDelete(IEntity<Schedule> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeCreate(IEntity<Schedule> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeDelete(IEntity<Schedule> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void beforeUpdate(IEntity<Schedule> arg0, UserSession arg1,
			ServiceData arg2) throws DvoException, BusinessException {
		// TODO Auto-generated method stub
		
	}

}