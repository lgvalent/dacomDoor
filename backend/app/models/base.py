from app import db


class CRUD():
	autoCommit = True
	@staticmethod
	def begin():
		CRUD.autoCommit = False
	
	@staticmethod
	def commit():
		CRUD.autoCommit = True
		return db.session.commit()

	def add(self, resource):
		db.session.add(resource)
		return db.session.commit() if CRUD.autoCommit else True

	def update(self):
		return db.session.commit() if CRUD.autoCommit else True

	def delete(self, resource):
		db.session.delete(resource)
		return db.session.commit() if CRUD.autoCommit else True
