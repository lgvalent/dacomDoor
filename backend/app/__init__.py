from flask import Flask
from flask_restful import Api
from flask_sqlalchemy import SQLAlchemy
from flask_jwt import JWT
from flask_cors import CORS


app = Flask(__name__)
api = Api(app)

app.config.from_object("config")

cors = CORS(app)
db = SQLAlchemy(app)

from app.models.admin import Admin


def authenticate(email, password):
	email = Admin.query.filter(Admin.email == email).first()
	if email and email.verify_password(password):
		return email


def identity(payload):
	user_id = payload["identity"]
	return Admin.query.get(user_id).email


jwt = JWT(app, authenticate, identity)
