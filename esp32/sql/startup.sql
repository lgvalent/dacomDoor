DROP TABLE IF EXISTS events;
CREATE TABLE IF NOT EXISTS events (
  id INTEGER NOT NULL,
  uid varchar(16) NOT NULL,
  time datetime NOT NULL,
  eventType varchar(4) NOT NULL,
  PRIMARY KEY (id)
);

DROP TABLE IF EXISTS keyrings;
CREATE TABLE IF NOT EXISTS keyrings (
  id INTEGER NOT NULL,
  uid varchar(16) NOT NULL,
  userId INTEGER NOT NULL,
  userType varchar(10) NOT NULL,
  lastUpdate datetime NOT NULL,
  PRIMARY KEY (id),
);

DROP TABLE IF EXISTS schedules;
CREATE TABLE IF NOT EXISTS schedules (
  id INTEGER NOT NULL,
  dayOfWeek varchar(10) NOT NULL,
  beginTime time NOT NULL,
  endTime time NOT NULL,
  userType varchar(10) NOT NULL,
  lastUpdate datetime NOT NULL,
  PRIMARY KEY (id)
);

INSERT INTO keyrings VALUES (1,'28:D0:9F:59:3E',1,'PROFESSOR','0000-00-00 00:00:00'),(2,'F6:B0:91:89:5E',1,'STUDENT','0000-00-00 00:00:00'),(3,'1A008D81DDCB',1,'PROFESSOR','0000-00-00 00:00:00');
INSERT INTO schedules VALUES (1,'SUNDAY','00:00:00','23:00:00','STUDENT','0000-00-00 00:00:00');
