CREATE TABLE IF NOT EXISTS rfids_permitidos (
  id INTEGER,
  rfid varchar(255),
  tipo varchar(255),
  PRIMARY KEY (id)
);

CREATE TABLE IF NOT EXISTS horarios_permitidos (
  id INTEGER,
  dia varchar(32),
  tipo_usuario varchar(255),
  hora_inicio TIME,
  hora_fim TIME,
  PRIMARY KEY (id)
);

CREATE TABLE IF NOT EXISTS eventos (
  id INTEGER,
  rfid varchar(255),
  tipo varchar(255),
  horario DATETIME,
  PRIMARY KEY (id)
);
