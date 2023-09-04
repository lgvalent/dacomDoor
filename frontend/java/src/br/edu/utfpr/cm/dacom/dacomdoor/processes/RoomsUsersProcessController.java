package br.edu.utfpr.cm.dacom.dacomdoor.processes;

import br.com.orionsoft.monstrengo.core.process.IProcess;
import br.com.orionsoft.monstrengo.core.process.IRunnableEntityProcessController;
import br.com.orionsoft.monstrengo.core.process.ProcessException;
import br.com.orionsoft.monstrengo.core.process.RunnableProcessControllerBasic;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.Room;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.RoomUser;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.User;

/**
 * @author Lucio 20200322
 */
public class RoomsUsersProcessController extends RunnableProcessControllerBasic implements IRunnableEntityProcessController {
	public static final Class<?>[] RUNNABLE_ENTITIES = {Room.class, User.class, RoomUser.class};

	public Class<? extends IProcess> getProcessClass() {return RoomsUsersProcess.class;}

	public boolean canRunWithEntity(IEntity<?> entity) throws ProcessException {
		return true;
	}

	public Class<?>[] getRunnableEntities() {
		return RUNNABLE_ENTITIES;
	}

}
