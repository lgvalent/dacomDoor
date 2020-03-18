package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

import org.hibernate.annotations.ForeignKey;

import br.com.orionsoft.monstrengo.crud.entity.dao.IDAO;

@Entity
@Table(name="roomsUsers")
public class RoomUser {

	private Long id = IDAO.ENTITY_UNSAVED;
	
	private User user;
	private Room room;
	private String description;

	private Calendar lastUpdate;
	private boolean active = true;
	
	@Id @GeneratedValue(strategy = GenerationType.IDENTITY)
	public Long getId() {return id;}
	public void setId(Long id) {this.id = id;}
	
	@ManyToOne
	@JoinColumn(name = "userId")
	@ForeignKey(name = "userId")
	public User getUser() {return user;}
	public void setUser(User user) {this.user = user;}
	
	@ManyToOne
	@JoinColumn(name = "roomId")
	@ForeignKey(name = "roomId")
	public Room getRoom() {return room;}
	public void setRoom(Room room) {this.room = room;}
	
	@Column(length=255)
	public String getDescription() {return description;}
	public void setDescription(String description) {this.description = description;}
	
	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getLastUpdate() {return lastUpdate;}
	public void setLastUpdate(Calendar lastUpdate) {this.lastUpdate = lastUpdate;}
	
	@Column
	public boolean isActive() {return active;}
	public void setActive(boolean active) {this.active = active;}

	@Override
	public String toString() {
		return (active?"":"(Inactive)") + room==null?"NULL":room.toString();
	}
}
