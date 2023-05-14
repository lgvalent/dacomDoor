package br.edu.utfpr.cm.dacom.dacomdoor.processes;

import java.util.Calendar;

import br.com.orionsoft.monstrengo.core.annotations.ProcessMetadata;
import br.com.orionsoft.monstrengo.core.exception.BusinessMessage;
import br.com.orionsoft.monstrengo.core.process.IRunnableEntityCollectionProcess;
import br.com.orionsoft.monstrengo.core.process.IRunnableEntityProcess;
import br.com.orionsoft.monstrengo.core.process.ProcessBasic;
import br.com.orionsoft.monstrengo.core.process.ProcessException;
import br.com.orionsoft.monstrengo.core.process.ProcessParamEntityList;
import br.com.orionsoft.monstrengo.core.service.ServiceData;
import br.com.orionsoft.monstrengo.core.service.ServiceException;
import br.com.orionsoft.monstrengo.core.util.CalendarUtils;
import br.com.orionsoft.monstrengo.crud.entity.EntityException;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.IEntityCollection;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.Room;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.RoomUser;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.User;
import br.edu.utfpr.cm.dacom.dacomdoor.services.RoomsUsersService;
import br.edu.utfpr.cm.dacom.dacomdoor.services.RoomsUsersService.Operation;

/**
 * Este processo cancela os lançamentos pendentes listados, gerando movimentos e alterando a situação.
 * 
 * @author Lucio
 * @version 20200322
 */
@ProcessMetadata(label="Garantir/Revogar acesso às salas", hint="Permite selecionar usuários e salas e garantir ou revogar seus acessos", description="Selecione os usuários e as salas para garantir ou revogar os acessos.<br/> A data de definição é importante para definições futuras, que não entrarão em vigor, imediatamente.<br/>O motivo do acesso também é uma informação importante para indicar por que o usuário acessa à sala e quem autorizou o seu acesso.")
public class RoomsUsersProcess extends ProcessBasic implements IRunnableEntityProcess, IRunnableEntityCollectionProcess {
	public static final String PROCESS_NAME="RoomsUsersProcess";

	private Calendar date = CalendarUtils.getCalendar();
	private ProcessParamEntityList<User> paramUsers = new ProcessParamEntityList<User>(User.class, false, this);
	private ProcessParamEntityList<Room> paramRooms = new ProcessParamEntityList<Room>(Room.class, false, this);

	private String description = "";

	@Override
	public void start() throws ProcessException {
		/* Seta a date inicial como primeiro dia do mês corrente */
		super.start();

		this.paramRooms.setStaticHqlWhereFilter("("+User.ACTIVE+ " = TRUE)");

		this.paramUsers.setStaticHqlWhereFilter("("+User.ACTIVE+ " = TRUE)");
	}


	private boolean run(Operation operation){

		try {
			ServiceData sdc = new ServiceData(RoomsUsersService.SERVICE_NAME, null);
			sdc.getArgumentList().setProperty(RoomsUsersService.IN_USERS, this.paramUsers.getValue());
			sdc.getArgumentList().setProperty(RoomsUsersService.IN_ROOMS, this.paramRooms.getValue());
			sdc.getArgumentList().setProperty(RoomsUsersService.IN_DATE, this.date);
			sdc.getArgumentList().setProperty(RoomsUsersService.IN_DESCRIPTION_OPT, this.description);
			sdc.getArgumentList().setProperty(RoomsUsersService.IN_OPERATION, operation);
			this.getProcessManager().getServiceManager().execute(sdc);

			/* Pega as mensagens do serviço */
			this.getMessageList().addAll(sdc.getMessageList());
			return true;
		} catch (ServiceException e) {
			/* Armazenando a lista de erros */
			this.getMessageList().addAll(e.getErrorList());
			return false;
		}
	}

	public boolean runGrant() {
		super.beforeRun();

		return run(Operation.GRANT);
	}

	public boolean runRevoke() {
		super.beforeRun();

		return run(Operation.REVOKE);
	}

	public String getProcessName() {
		return PROCESS_NAME;
	}

	public Calendar getDate() {return date;}
	public void setDate(Calendar date) {this.date = date;}

	public String getDescription() {return description;}
	public void setDescription(String description) {this.description = description;}

	public ProcessParamEntityList<User> getParamUsers(){return this.paramUsers;}

	public ProcessParamEntityList<Room> getParamRooms(){return this.paramRooms;}

	@SuppressWarnings("unchecked")
	public boolean runWithEntity(IEntity<?> entity) {
		super.beforeRun();
		boolean result = false;
		if (entity.getInfo().getType() == User.class) {
			this.paramUsers.getValue().add((IEntity<User>) entity);
			result = true;
		} else 
			if (entity.getInfo().getType() == Room.class) {
				this.paramRooms.getValue().add((IEntity<Room>) entity);
				result = true;
			} else
				if (entity.getInfo().getType() == RoomUser.class) {
					try {
						this.paramUsers.getValue().add((IEntity<User>) entity.getPropertyValue(RoomUser.USER));
						this.paramRooms.getValue().add((IEntity<Room>) entity.getPropertyValue(RoomUser.ROOM));
					} catch (EntityException e) {
						result = false;
					}
					result = true;
				} else {
				this.getMessageList().add(new BusinessMessage(IRunnableEntityProcess.class, "ENTITY_NOT_COMPATIBLE", PROCESS_NAME, entity.getInfo().getType().getName()));
			}
		return result;
	}

	@SuppressWarnings("unchecked")
	@Override
	public boolean runWithEntities(IEntityCollection<?> entities) {
		super.beforeRun();
		boolean result = false;
		if (entities.getInfo().getType() == User.class) {
			this.paramUsers.getValue().addAll((IEntityCollection<User>) entities);
			result = true;
		} else
			if (entities.getInfo().getType() == Room.class) {
				this.paramRooms.getValue().addAll((IEntityCollection<Room>) entities);
				result = true;
			} else 
				if (entities.getInfo().getType() == RoomUser.class) {
					for(IEntity<?> entity: entities) {
						try {
							this.paramUsers.getValue().add((IEntity<User>) entity.getPropertyValue(RoomUser.USER));
							this.paramRooms.getValue().add((IEntity<Room>) entity.getPropertyValue(RoomUser.ROOM));
						} catch (EntityException e) {
							result = false;
						}
					}
					result = true;
				} else {
				this.getMessageList().add(new BusinessMessage(IRunnableEntityProcess.class, "ENTITY_NOT_COMPATIBLE", PROCESS_NAME, entities.getInfo().getType().getName()));
			}
		return result;
	}

}