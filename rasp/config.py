# WAN Access
#URL_SERVER = "http://dacom.cm.utfpr.edu.br:19500"
# LAN Access
URL_SERVER = "http://dacomdoor.dacom:5000"
ROOM_NAME = "CAFE"
ROOM_LAST_UPDATE_FAKE = "2001-01-01 00:00:00"
UPDATE_DELAY = 60
BOARD_VERSION = 6
# if >1 define magnetic lock else electric lock
RELAY_DELAY = .1
DOOR_OPENED_ALERT_DELAY = 30

# SQLAlchemy
SQLALCHEMY_ECHO = False
SQLALCHEMY_TRACK_MODIFICATIONS = True

# MYSQL SETTINGS
DB_HOST = "localhost"
DB_SCHEMA = "dacomDoor"
DB_USERNAME = "user"
DB_PASSWORD = "user"

# MySQL + SQLAlchemy
SQLALCHEMY_DATABASE_URI = ("mysql+pymysql://{DB_USER}:{DB_PASS}@{DB_ADDR}/{DB_NAME}"
							.format(DB_USER=DB_USERNAME, DB_PASS=DB_PASSWORD,
									DB_ADDR=DB_HOST, DB_NAME=DB_SCHEMA))
# SQL Commands to create SCHEMA and USER
# CREATE DATABASE dacomDoor;
# CREATE USER 'user'@'localhost' IDENTIFIED BY 'user';
# GRANT ALL PRIVILEGES ON dacomDoor.* TO 'user'@'localhost';
