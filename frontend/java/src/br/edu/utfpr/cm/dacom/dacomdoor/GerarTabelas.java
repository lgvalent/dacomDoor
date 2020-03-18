package br.edu.utfpr.cm.dacom.dacomdoor;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import org.hibernate.cfg.AnnotationConfiguration;
import org.hibernate.tool.hbm2ddl.SchemaExport;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import br.com.orionsoft.monstrengo.RewriteConstraintExportSchema;
import br.com.orionsoft.monstrengo.core.test.ServiceBasicTest;
import br.com.orionsoft.monstrengo.crud.entity.EntityException;
import br.com.orionsoft.monstrengo.crud.entity.IEntity;
import br.com.orionsoft.monstrengo.crud.entity.PropertyValueException;
import br.com.orionsoft.monstrengo.crud.entity.dao.IDAO;
import br.com.orionsoft.monstrengo.crud.services.UtilsCrud;
import br.edu.utfpr.cm.dacom.dacomdoor.entities.Schedule;

/**
 * Esta classe gera o banco de acordo com as classes anotadas no applicationContext.xml.
 * Nos projetos filhos, esta classe poderá ser extendida e os atributos poderão ser sobrescritos os valores.
 * @author lucio 
 */
public class GerarTabelas extends ServiceBasicTest{
	
	@Before
	public void setUp() throws Exception {
		super.setUp();
	}
	
	@After
	public void tearDown() throws Exception {
		super.tearDown();
	}

//	@Test
//	public void testCreateSchema() {
//		createSchema(true, false, false);
//	}

	@Test
	public void testCreateSchema() throws Exception {
		IEntity<Schedule> s =  UtilsCrud.create(serviceManager, Schedule.class, null);
		s.getProperty("endTime").getValue().setAsString("20:00");
		System.out.println(s);
	}
	
	public static void main(String[] args) throws Throwable {
    	String mask = "HH:mm";
    	String value = "20:00";
		Calendar cal = Calendar.getInstance();
        
        cal.setTime(new SimpleDateFormat(mask).parse(value));
        
        System.out.println(cal);


        
//		new org.junit.runners.AllTests(GerarTabelas.class).testCount();
	}

	/**
	 * Este método é utilizado pelas classes filhas para executar uma geração personalizada.
	 */
//	public void createSchema(boolean generateScript,
//							  boolean export,
//							  boolean justDrop) {
//		AnnotationConfiguration cfg = new AnnotationConfiguration();
//
//		cfg.setProperties(this.daoManager.getConfiguration().getProperties());
//		
//		for(IDAO<?> dao: this.daoManager.getDaos().values()){
//			cfg.addAnnotatedClass(dao.getEntityClass());
//		}
//		
//		SchemaExport schExport = new SchemaExport(cfg);
//		/* É necessário que o arquivo esteja com edição liberada. 
//		 * Para isto, acesse o popup menu>Team>Edit */
//		schExport.setOutputFile("./schema-export.sql");
//		schExport.execute(generateScript, export, justDrop, true);
//
//		if(generateScript){
//			/* Reescreve o schema-export.sql para renomear as constraint */
//			new RewriteConstraintExportSchema("./schema-export.sql").runRewriteFile();
//		}
//	}
//	

}
