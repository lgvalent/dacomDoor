from app import app
from app.routes import *


if __name__ == '__main__':
	app.run(
		host=app.config["HOST"],
		port=app.config["PORT"],
		debug=app.config["DEBUG"]
	)
