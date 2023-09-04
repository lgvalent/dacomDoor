/*
 * Created on 31/03/2006 by antonio
 */
package br.edu.utfpr.cm.dacom.dacomdoor.views;

import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;

import br.com.orionsoft.monstrengo.core.exception.BusinessException;
import br.com.orionsoft.monstrengo.core.process.ProcessException;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.IEntityCollection;
import br.com.orionsoft.monstrengo.crud.entity.IEntityList;
import br.com.orionsoft.monstrengo.view.jsf.bean.BeanSessionBasic;
import br.com.orionsoft.monstrengo.view.jsf.bean.IRunnableProcessView;
import br.com.orionsoft.monstrengo.view.jsf.util.FacesUtils;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.User;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.UserType;
import br.edu.utfpr.cm.dacom.dacomdoor.processes.RoomsUsersProcess;

/**
 * Bean que cotrola a garantia e revogação de acesso aos usuários.
 * 
 */
@ManagedBean
@SessionScoped
public class RoomsUsersBean extends BeanSessionBasic implements IRunnableProcessView {
	private static final long serialVersionUID = 1L;

	/** Define a view JSF que é ativada para cada view */
	public static final String FACES_VIEW_ROOMS_USERS = "/pages/dacomdoor/processRoomsUsers?faces-redirect=true";
	public static final String VIEW_NAME = "roomsUsersBean";

    private RoomsUsersProcess process = null;
    
    /**
     * Este método tenta ler os parâmetros da atual requisição.
     * Se ele conseguir ele pega os valores, aplica no processo
     * e retorna true informando que os parâmetros foram encontrados.
     * Assim, os actions poderão decidir se passa para um ou para outro 
     * passo já que os parâmetros já foram fornecidos.
     * @throws BusinessException
     */
    public boolean loadParams() throws BusinessException {
    	boolean result = false;
        return result;
    }

    public void doReset() throws BusinessException, Exception {}

    public void doReload() throws BusinessException, Exception {}
    
    public RoomsUsersProcess getProcess() throws ProcessException {
        if (process == null)
            process = (RoomsUsersProcess)this.getApplicationBean().getProcessManager().createProcessByName(RoomsUsersProcess.PROCESS_NAME, this.getUserSessionBean().getUserSession());
        return process;
    }
    
    public String doGrant() throws ProcessException {
        log.debug("ProcessRoomsUsers.doGrant");
        
        if (getProcess().runGrant()) {
            // Adiciona as mensagens de info no Faces
            FacesUtils.addInfoMsgs(process.getMessageList());
                      
            // Definir o fluxo de tela
            return FacesUtils.FACES_VIEW_SUCCESS;
        }else{
            // Adiciona as mensagens de erro no Faces
            FacesUtils.addErrorMsgs(process.getMessageList());
            // Definir o fluxo de tela
            return FacesUtils.FACES_VIEW_FAILURE;
        }
    }

    public String doRevoke() throws ProcessException {
        log.debug("ProcessRoomsUsers.doGrant");
        
        if (getProcess().runRevoke()) {
            // Adiciona as mensagens de info no Faces
            FacesUtils.addInfoMsgs(process.getMessageList());
                      
            // Definir o fluxo de tela
            return FacesUtils.FACES_VIEW_SUCCESS;
        }else{
            // Adiciona as mensagens de erro no Faces
            FacesUtils.addErrorMsgs(process.getMessageList());
            // Definir o fluxo de tela
            return FacesUtils.FACES_VIEW_FAILURE;
        }
    }
    
    public void doAllByUserType(String userTypeName) throws ProcessException {
    	UserType userType =  UserType.valueOf(userTypeName);
    	
    	for(IEntity<User> user: getProcess().getParamUsers().getFullList()) {
    		if(user.getObject().getUserType() == userType)
    			getProcess().getParamUsers().getValue().add(user);
    	}
    }

	/* IRunnableProcessView */
	public String getViewName() {
		return VIEW_NAME;
	}
	
    @Override
    public String actionStart() {
		return FACES_VIEW_ROOMS_USERS;
    }
	
	public String runWithEntities(IEntityList<?> entities) {
		try {
			this.getProcess().runWithEntities(entities);
		} catch (ProcessException e) {
			FacesUtils.addErrorMsgs(e.getErrorList());
			return FacesUtils.FACES_VIEW_FAILURE;
		}
		return FACES_VIEW_ROOMS_USERS; // O lançamento já está preenchido, vai par ao segujndo passo!
	}

	@Override
	public String getRunnableEntityProcessName() {
		return RoomsUsersProcess.PROCESS_NAME;
	}

	@Override
	public String runWithEntity(IEntity<?> entity) {
		try {
			this.getProcess().runWithEntity((IEntity<?>) entity);
		} catch (ProcessException e) {
			FacesUtils.addErrorMsgs(e.getErrorList());
			return FacesUtils.FACES_VIEW_FAILURE;
		}
		return FACES_VIEW_ROOMS_USERS; // O lançamento já está preenchido, vai par ao segujndo passo!
	}

	public String runWithEntities(IEntityCollection<?> entities) {
		try {
			this.getProcess().runWithEntities(entities);
		} catch (ProcessException e) {
			FacesUtils.addErrorMsgs(e.getErrorList());
			return FacesUtils.FACES_VIEW_FAILURE;
		}
		return FACES_VIEW_ROOMS_USERS; // O lançamento já está preenchido, vai par ao segujndo passo!
	}

	
	
}
