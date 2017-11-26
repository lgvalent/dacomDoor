nome_sala_rasp = "E101"
last_update_fake = "2001-01-01T00:00:00+00:00"

# SQLAlchemy
SQLALCHEMY_ECHO = True
SQLALCHEMY_TRACK_MODIFICATIONS = True

# MYSQL - TROCAR NOMES
mysql_db_username = "kds"
mysql_db_password = ""
mysql_db_name = "rasp"
mysql_db_hostname = "localhost"

# MySQL + SQLAlchemy
SQLALCHEMY_DATABASE_URI = ("mysql+pymysql://{DB_USER}:{DB_PASS}@{DB_ADDR}/{DB_NAME}"
							.format(DB_USER=mysql_db_username, DB_PASS=mysql_db_password,
									DB_ADDR=mysql_db_hostname, DB_NAME=mysql_db_name))
