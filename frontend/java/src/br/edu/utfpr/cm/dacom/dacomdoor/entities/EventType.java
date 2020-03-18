package br.edu.utfpr.cm.dacom.dacomdoor.entities;

public enum EventType {
	IN(1, "get in"),
    OUT(2, "get out");

	public static final int COLUMN_DISCRIMINATOR_LENGTH = 3;

	private final int id;
	private final String nome;

	private EventType(final int id, final String nome) {
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
