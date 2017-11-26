from flask import jsonify


def send_message(message, status_code=404):
	response = jsonify({"message": message})
	response.status_code = status_code
	return response


def rollback(error, db):
	db.session.rollback()
	return send_message(str(error), 403)
