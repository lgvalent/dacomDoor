--<ScriptOptions statementTerminator=";"/>

ALTER TABLE security_right_crud DROP PRIMARY KEY;

ALTER TABLE auditorship_crud DROP PRIMARY KEY;

ALTER TABLE horario DROP PRIMARY KEY;

ALTER TABLE rooms DROP PRIMARY KEY;

ALTER TABLE security_module DROP PRIMARY KEY;

ALTER TABLE security_process DROP PRIMARY KEY;

ALTER TABLE framework_label_model DROP PRIMARY KEY;

ALTER TABLE security_user_group DROP PRIMARY KEY;

ALTER TABLE events DROP PRIMARY KEY;

ALTER TABLE security_right_process DROP PRIMARY KEY;

ALTER TABLE auditorship_process DROP PRIMARY KEY;

ALTER TABLE framework_query_condiction DROP PRIMARY KEY;

ALTER TABLE framework_label_model_entity DROP PRIMARY KEY;

ALTER TABLE framework_email_account DROP PRIMARY KEY;

ALTER TABLE framework_parent_condiction DROP PRIMARY KEY;

ALTER TABLE users DROP PRIMARY KEY;

ALTER TABLE alembic_version DROP PRIMARY KEY;

ALTER TABLE eventos DROP PRIMARY KEY;

ALTER TABLE framework_label_address_group DROP PRIMARY KEY;

ALTER TABLE framework_result_condiction DROP PRIMARY KEY;

ALTER TABLE security_group DROP PRIMARY KEY;

ALTER TABLE sala DROP PRIMARY KEY;

ALTER TABLE usuario DROP PRIMARY KEY;

ALTER TABLE security_user DROP PRIMARY KEY;

ALTER TABLE schedules DROP PRIMARY KEY;

ALTER TABLE admin DROP PRIMARY KEY;

ALTER TABLE security_entity_property_group DROP PRIMARY KEY;

ALTER TABLE direito_acesso DROP PRIMARY KEY;

ALTER TABLE security_entity_property DROP PRIMARY KEY;

ALTER TABLE framework_label_address DROP PRIMARY KEY;

ALTER TABLE auditorship_service DROP PRIMARY KEY;

ALTER TABLE framework_user_report DROP PRIMARY KEY;

ALTER TABLE security_entity DROP PRIMARY KEY;

ALTER TABLE framework_order_condiction DROP PRIMARY KEY;

ALTER TABLE roomsUsers DROP PRIMARY KEY;

ALTER TABLE framework_document_model_entity DROP PRIMARY KEY;

ALTER TABLE framework_page_condiction DROP PRIMARY KEY;

ALTER TABLE framework_label_address DROP INDEX applicationUser;

ALTER TABLE security_entity DROP INDEX applicationModule;

ALTER TABLE framework_document_model_entity DROP INDEX applicationEntity;

ALTER TABLE events DROP INDEX userId;

ALTER TABLE auditorship_process DROP INDEX applicationProcess;

ALTER TABLE security_user_group DROP INDEX applicationGroup;

ALTER TABLE framework_query_condiction DROP INDEX userReport;

ALTER TABLE framework_order_condiction DROP INDEX userReport;

ALTER TABLE horario DROP INDEX id_sala;

ALTER TABLE schedules DROP INDEX roomId;

ALTER TABLE framework_user_report DROP INDEX applicationEntity;

ALTER TABLE security_right_crud DROP INDEX securityGroup;

ALTER TABLE security_right_process DROP INDEX securityGroup;

ALTER TABLE eventos DROP INDEX id_sala;

ALTER TABLE framework_document_model_entity DROP INDEX applicationUser;

ALTER TABLE security_entity_property DROP INDEX applicationEntity;

ALTER TABLE security_user DROP INDEX login;

ALTER TABLE framework_parent_condiction DROP INDEX applicationEntity;

ALTER TABLE auditorship_crud DROP INDEX applicationEntity;

ALTER TABLE sala DROP INDEX nome;

ALTER TABLE auditorship_crud DROP INDEX applicationUser;

ALTER TABLE security_right_process DROP INDEX applicationProcess;

ALTER TABLE framework_label_address_group DROP INDEX applicationUser;

ALTER TABLE direito_acesso DROP INDEX id_usuario;

ALTER TABLE usuario DROP INDEX rfid;

ALTER TABLE auditorship_process DROP INDEX applicationUser;

ALTER TABLE auditorship_service DROP INDEX applicationUser;

ALTER TABLE framework_user_report DROP INDEX applicationUser;

ALTER TABLE roomsUsers DROP INDEX userId;

ALTER TABLE security_process DROP INDEX applicationModule;

ALTER TABLE framework_result_condiction DROP INDEX userReport;

ALTER TABLE security_right_crud DROP INDEX applicationEntity;

ALTER TABLE framework_label_address DROP INDEX applicationEntity;

ALTER TABLE security_user_group DROP INDEX applicationUser;

ALTER TABLE admin DROP INDEX email;

ALTER TABLE events DROP INDEX roomId;

ALTER TABLE framework_label_model_entity DROP INDEX applicationEntity;

ALTER TABLE framework_label_address DROP INDEX addressLabelGroup;

ALTER TABLE direito_acesso DROP INDEX id_sala;

ALTER TABLE security_entity_property_group DROP INDEX applicationEntity;

ALTER TABLE eventos DROP INDEX id_usuario;

ALTER TABLE roomsUsers DROP INDEX roomId;

ALTER TABLE usuario DROP INDEX email;

DROP TABLE horario;

DROP TABLE usuario;

DROP TABLE security_right_process;

DROP TABLE security_entity_property_group;

DROP TABLE events;

DROP TABLE auditorship_crud;

DROP TABLE alembic_version;

DROP TABLE framework_document_model_entity;

DROP TABLE roomsUsers;

DROP TABLE sala;

DROP TABLE framework_label_model;

DROP TABLE framework_user_report;

DROP TABLE admin;

DROP TABLE framework_label_address_group;

DROP TABLE security_user;

DROP TABLE security_entity_property;

DROP TABLE security_process;

DROP TABLE security_module;

DROP TABLE auditorship_service;

DROP TABLE eventos;

DROP TABLE framework_label_address;

DROP TABLE security_group;

DROP TABLE framework_page_condiction;

DROP TABLE rooms;

DROP TABLE security_user_group;

DROP TABLE framework_label_model_entity;

DROP TABLE framework_order_condiction;

DROP TABLE schedules;

DROP TABLE security_entity;

DROP TABLE framework_query_condiction;

DROP TABLE security_right_crud;

DROP TABLE framework_parent_condiction;

DROP TABLE auditorship_process;

DROP TABLE users;

DROP TABLE framework_result_condiction;

DROP TABLE framework_email_account;

DROP TABLE direito_acesso;

CREATE TABLE horario (
	id INT NOT NULL,
	last_update DATETIME NOT NULL,
	dia ENUM NOT NULL,
	hora_fim TIME NOT NULL,
	hora_inicio TIME NOT NULL,
	tipo_user ENUM NOT NULL,
	id_sala INT NOT NULL,
	alive BIT NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE usuario (
	id INT NOT NULL,
	nome VARCHAR(80) NOT NULL,
	tipo ENUM NOT NULL,
	email VARCHAR(100) NOT NULL,
	rfid VARCHAR(16) NOT NULL,
	last_update DATETIME NOT NULL,
	alive BIT NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_right_process (
	id BIGINT NOT NULL,
	executeAllowed BIT,
	securityGroup BIGINT,
	applicationProcess BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_entity_property_group (
	id BIGINT NOT NULL,
	colorName VARCHAR(50),
	description TEXT,
	hint VARCHAR(255),
	indexGroup INT,
	label VARCHAR(100),
	name VARCHAR(100),
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE events (
	id BIGINT NOT NULL,
	dateTime DATETIME,
	eventType VARCHAR(3),
	roomId BIGINT,
	userId BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE auditorship_crud (
	id BIGINT NOT NULL,
	description VARCHAR(1000),
	ocurrencyDate DATETIME,
	terminal VARCHAR(255),
	created BIT,
	deleted BIT,
	entityId BIGINT,
	updated BIT,
	applicationUser BIGINT,
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE alembic_version (
	version_num VARCHAR(32) NOT NULL,
	PRIMARY KEY (version_num)
) ENGINE=InnoDB;

CREATE TABLE framework_document_model_entity (
	id BIGINT NOT NULL,
	date DATETIME,
	description VARCHAR(255),
	name VARCHAR(50),
	source TEXT,
	applicationEntity BIGINT,
	applicationUser BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE roomsUsers (
	id BIGINT NOT NULL,
	roomId BIGINT,
	userId BIGINT,
	active BIT,
	lastUpdate DATETIME,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE sala (
	id INT NOT NULL,
	nome VARCHAR(20),
	last_update DATETIME NOT NULL,
	alive BIT NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_label_model (
	id BIGINT NOT NULL,
	envelope BIT,
	fontName VARCHAR(50),
	fontSize INT,
	horizontalDistance FLOAT,
	labelHeight FLOAT,
	labelWidth FLOAT,
	marginLeft FLOAT,
	marginTop FLOAT,
	name VARCHAR(50),
	pageHeight FLOAT,
	pageWidth FLOAT,
	verticalDistance FLOAT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_user_report (
	id BIGINT NOT NULL,
	date DATETIME,
	description TEXT,
	filterCondiction VARCHAR(50),
	hqlWhereCondiction TEXT,
	name VARCHAR(100),
	applicationUser BIGINT,
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE admin (
	id INT NOT NULL,
	nome VARCHAR(80) NOT NULL,
	email VARCHAR(100) NOT NULL,
	password VARCHAR(128) NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_label_address_group (
	id BIGINT NOT NULL,
	name VARCHAR(100),
	applicationUser BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_user (
	id BIGINT NOT NULL,
	inactive BIT,
	login VARCHAR(20),
	name VARCHAR(50),
	password VARCHAR(50),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_entity_property (
	id BIGINT NOT NULL,
	allowSubQuery BIT,
	colorName VARCHAR(50),
	defaultValue VARCHAR(50),
	description TEXT,
	displayFormat VARCHAR(50),
	editMask VARCHAR(50),
	editShowList BIT,
	hint VARCHAR(255),
	indexGroup INT,
	indexProperty INT,
	label VARCHAR(100),
	maximum DOUBLE,
	minimum DOUBLE,
	name VARCHAR(50),
	readOnly BIT,
	required BIT,
	valuesList VARCHAR(255),
	visible BIT,
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_process (
	id BIGINT NOT NULL,
	description TEXT,
	hint VARCHAR(255),
	label VARCHAR(100),
	name VARCHAR(50),
	applicationModule BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_module (
	id BIGINT NOT NULL,
	name VARCHAR(50),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE auditorship_service (
	id BIGINT NOT NULL,
	description VARCHAR(1000),
	ocurrencyDate DATETIME,
	terminal VARCHAR(255),
	serviceName VARCHAR(50),
	applicationUser BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE eventos (
	id INT NOT NULL,
	evento ENUM NOT NULL,
	horario DATETIME NOT NULL,
	id_usuario INT NOT NULL,
	id_sala INT NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_label_address (
	id BIGINT NOT NULL,
	line1 VARCHAR(150),
	line2 VARCHAR(150),
	line3 VARCHAR(150),
	line4 VARCHAR(150),
	line5 VARCHAR(150),
	ocurrencyDate DATETIME,
	print BIT,
	applicationUser BIGINT,
	addressLabelGroup BIGINT,
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_group (
	id BIGINT NOT NULL,
	name VARCHAR(50),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_page_condiction (
	id BIGINT NOT NULL,
	itemsCount INT,
	page INT,
	pageSize INT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE rooms (
	id BIGINT NOT NULL,
	active BIT,
	lastUpdate DATETIME,
	name VARCHAR(20),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_user_group (
	applicationUser BIGINT NOT NULL,
	securityGroup BIGINT NOT NULL,
	PRIMARY KEY (securityGroup,applicationUser)
) ENGINE=InnoDB;

CREATE TABLE framework_label_model_entity (
	id BIGINT NOT NULL,
	description VARCHAR(255),
	line1 VARCHAR(255),
	line2 VARCHAR(255),
	line3 VARCHAR(255),
	line4 VARCHAR(255),
	line5 VARCHAR(255),
	name VARCHAR(50),
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_order_condiction (
	id BIGINT NOT NULL,
	active BIT,
	orderDirection INT,
	propertyPath VARCHAR(200),
	userReport BIGINT,
	orderIndex INT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE schedules (
	id BIGINT NOT NULL,
	active BIT,
	beginTime TIME,
	dayOfWeek VARCHAR(9),
	endTime TIME,
	lastUpdate DATETIME,
	userType VARCHAR(9),
	roomId BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_entity (
	id BIGINT NOT NULL,
	className VARCHAR(200),
	colorName VARCHAR(50),
	description TEXT,
	hint VARCHAR(255),
	label VARCHAR(100),
	name VARCHAR(100),
	runQueryOnOpen BIT,
	applicationModule BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_query_condiction (
	id BIGINT NOT NULL,
	active BIT,
	closePar BIT,
	initOperator INT,
	openPar BIT,
	operatorId INT,
	propertyPath VARCHAR(200),
	value1 VARCHAR(50),
	value2 VARCHAR(50),
	userReport BIGINT,
	orderIndex INT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE security_right_crud (
	id BIGINT NOT NULL,
	createAllowed BIT,
	deleteAllowed BIT,
	queryAllowed BIT,
	retrieveAllowed BIT,
	updateAllowed BIT,
	securityGroup BIGINT,
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_parent_condiction (
	id BIGINT NOT NULL,
	property VARCHAR(50),
	applicationEntity BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE auditorship_process (
	id BIGINT NOT NULL,
	description VARCHAR(1000),
	ocurrencyDate DATETIME,
	terminal VARCHAR(255),
	applicationUser BIGINT,
	applicationProcess BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE users (
	id BIGINT NOT NULL,
	active BIT,
	email VARCHAR(100),
	lastUpdate DATETIME,
	name VARCHAR(80),
	uid VARCHAR(16),
	userType VARCHAR(9),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_result_condiction (
	id BIGINT NOT NULL,
	propertyPath VARCHAR(200),
	resultIndex INT,
	userReport BIGINT,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE framework_email_account (
	id BIGINT NOT NULL,
	host VARCHAR(50),
	password VARCHAR(50),
	properties VARCHAR(500),
	senderMail VARCHAR(100),
	senderName VARCHAR(100),
	useAsDefault BIT,
	user VARCHAR(50),
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE TABLE direito_acesso (
	id INT NOT NULL,
	id_usuario INT NOT NULL,
	id_sala INT NOT NULL,
	alive BIT NOT NULL,
	PRIMARY KEY (id)
) ENGINE=InnoDB;

CREATE INDEX applicationUser ON framework_label_address (applicationUser ASC);

CREATE INDEX applicationModule ON security_entity (applicationModule ASC);

CREATE INDEX applicationEntity ON framework_document_model_entity (applicationEntity ASC);

CREATE INDEX userId ON events (userId ASC);

CREATE INDEX applicationProcess ON auditorship_process (applicationProcess ASC);

CREATE INDEX applicationGroup ON security_user_group (securityGroup ASC);

CREATE INDEX userReport ON framework_query_condiction (userReport ASC);

CREATE INDEX userReport ON framework_order_condiction (userReport ASC);

CREATE INDEX id_sala ON horario (id_sala ASC);

CREATE INDEX roomId ON schedules (roomId ASC);

CREATE INDEX applicationEntity ON framework_user_report (applicationEntity ASC);

CREATE INDEX securityGroup ON security_right_crud (securityGroup ASC);

CREATE INDEX securityGroup ON security_right_process (securityGroup ASC);

CREATE INDEX id_sala ON eventos (id_sala ASC);

CREATE INDEX applicationUser ON framework_document_model_entity (applicationUser ASC);

CREATE INDEX applicationEntity ON security_entity_property (applicationEntity ASC);

CREATE UNIQUE INDEX login ON security_user (login ASC);

CREATE INDEX applicationEntity ON framework_parent_condiction (applicationEntity ASC);

CREATE INDEX applicationEntity ON auditorship_crud (applicationEntity ASC);

CREATE UNIQUE INDEX nome ON sala (nome ASC);

CREATE INDEX applicationUser ON auditorship_crud (applicationUser ASC);

CREATE INDEX applicationProcess ON security_right_process (applicationProcess ASC);

CREATE INDEX applicationUser ON framework_label_address_group (applicationUser ASC);

CREATE INDEX id_usuario ON direito_acesso (id_usuario ASC);

CREATE UNIQUE INDEX rfid ON usuario (rfid ASC);

CREATE INDEX applicationUser ON auditorship_process (applicationUser ASC);

CREATE INDEX applicationUser ON auditorship_service (applicationUser ASC);

CREATE INDEX applicationUser ON framework_user_report (applicationUser ASC);

CREATE INDEX userId ON roomsUsers (userId ASC);

CREATE INDEX applicationModule ON security_process (applicationModule ASC);

CREATE INDEX userReport ON framework_result_condiction (userReport ASC);

CREATE INDEX applicationEntity ON security_right_crud (applicationEntity ASC);

CREATE INDEX applicationEntity ON framework_label_address (applicationEntity ASC);

CREATE INDEX applicationUser ON security_user_group (applicationUser ASC);

CREATE UNIQUE INDEX email ON admin (email ASC);

CREATE INDEX roomId ON events (roomId ASC);

CREATE INDEX applicationEntity ON framework_label_model_entity (applicationEntity ASC);

CREATE INDEX addressLabelGroup ON framework_label_address (addressLabelGroup ASC);

CREATE INDEX id_sala ON direito_acesso (id_sala ASC);

CREATE INDEX applicationEntity ON security_entity_property_group (applicationEntity ASC);

CREATE INDEX id_usuario ON eventos (id_usuario ASC);

CREATE INDEX roomId ON roomsUsers (roomId ASC);

CREATE UNIQUE INDEX email ON usuario (email ASC);

