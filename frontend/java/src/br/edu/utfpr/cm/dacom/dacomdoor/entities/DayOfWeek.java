package br.edu.utfpr.cm.dacom.dacomdoor.entities;

public enum DayOfWeek {
	SUNDAY(1, "Sunday"),
    MONDAY(2, "Monday"),
    THUESDAY(3, "Thuesday"),
    WEDNESDAY(4, "Wednesday"),
    THURSDAY(5, "Thursday"),
    FRIDAY(6, "Friday"),
    SATURDAY(2, "Saturday");

	public static final int COLUMN_DISCRIMINATOR_LENGTH = 9;

	private final int id;
	private final String nome;

	private DayOfWeek(final int id, final String nome) {
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