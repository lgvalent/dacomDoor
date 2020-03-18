package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;
import java.util.HashSet;
import java.util.Set;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

import org.hibernate.annotations.ForeignKey;
import org.hibernate.annotations.LazyCollection;
import org.hibernate.annotations.LazyCollectionOption;

import br.com.orionsoft.monstrengo.crud.entity.dao.IDAO;

@Entity
@Table(name="rooms")
public class Room {

	private long id = IDAO.ENTITY_UNSAVED;

	private String name;

	private Calendar lastUpdate;
	private Calendar lastSynchronization;
	private String lastAddress;
	private boolean active = true;
	private Set<Schedule> schedules = new HashSet<Schedule>();

	@Id	@GeneratedValue(strategy = GenerationType.IDENTITY)
	public long getId() {return id;}
	public void setId(long id) {this.id = id;}
	
	@Column(length=20)
	public String getName() {return name;}
	public void setName(String name) {this.name = name;}
	
	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getLastUpdate() {return lastUpdate;}
	public void setLastUpdate(Calendar lastUpdate) {this.lastUpdate = lastUpdate;}
	
	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getLastSynchronization() {return lastSynchronization;}
	public void setLastSynchronization(Calendar lastSynchronization) {this.lastSynchronization = lastSynchronization;}
	
	@Column(length=100)
	public String getLastAddress() {return lastAddress;}
	public void setLastAddress(String lastAddress) {this.lastAddress = lastAddress;}
	
	@Column
	public boolean isActive() {return active;}
	public void setActive(boolean active) {this.active = active;}
	
	@OneToMany(cascade=CascadeType.ALL)
	@ForeignKey(name="roomId")
	@JoinColumn(name="roomId")
	@LazyCollection(LazyCollectionOption.FALSE)
	public Set<Schedule> getSchedules() {return schedules;}
	public void setSchedules(Set<Schedule> schedules) {this.schedules = schedules;}
	
	@Override
	public String toString() {
		return name;
	}
}
