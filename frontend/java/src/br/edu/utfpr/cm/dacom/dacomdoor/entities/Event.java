package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

import org.hibernate.annotations.ForeignKey;

import br.com.orionsoft.monstrengo.core.util.CalendarUtils;
import br.com.orionsoft.monstrengo.crud.entity.dao.IDAO;

@Entity
@Table(name="events")
public class Event {
	private long id = IDAO.ENTITY_UNSAVED;
	
	private EventType eventType;
	private Calendar dateTime;
	private User user;
	private Room room;
	
	@Id	@GeneratedValue(strategy = GenerationType.IDENTITY)
	public long getId() {return id;}
	public void setId(long id) {this.id = id;}
	
	@Enumerated(EnumType.STRING)
	@Column(length=EventType.COLUMN_DISCRIMINATOR_LENGTH)
	public EventType getEventType() {return eventType;}
	public void setEventType(EventType eventType) {this.eventType = eventType;}
	
	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getDateTime() {return dateTime;}
	public void setDateTime(Calendar dateTime) {this.dateTime = dateTime;}
	
	@ManyToOne @JoinColumn(name = "userId")	@ForeignKey(name = "user")
	public User getUser() {return user;}
	public void setUser(User user) {this.user = user;}

	@ManyToOne @JoinColumn(name = "roomId")	@ForeignKey(name = "room")
	public Room getRoom() {return room;}
	public void setRoom(Room room) {this.room = room;}
	
	@Override
	public String toString() {
		return this.room + " - " + this.user + " (" + CalendarUtils.formatDateTime(dateTime) + ")";
	}
}
