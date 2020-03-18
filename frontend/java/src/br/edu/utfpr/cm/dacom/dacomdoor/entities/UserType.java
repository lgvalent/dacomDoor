package br.edu.utfpr.cm.dacom.dacomdoor.entities;

public enum UserType {
	STUDENT(1, "Student"),
    PROFESSOR(2, "Professor"),
    EMPLOYEE(3, "Employee");

	public static final int COLUMN_DISCRIMINATOR_LENGTH = 9;

	private final int id;
	private final String nome;

	private UserType(final int id, final String nome) {
		this.id = id;
		this.nome = nome;
	}

	public int getId() {
		return id;
	}

	public String getNome() {
		return nome;
	}
	
	@Override
	public String toString() {
		return nome;
	}
}
