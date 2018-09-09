from flask import jsonify


def response(message, status_code=404):
	result = jsonify({"message": message})
	result.status_code = status_code
	return result


def rollback(error, db):
	db.session.rollback()
	return response(str(error), 403)
