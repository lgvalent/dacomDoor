package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;
import java.util.Date;

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
@Table(name="schedules")
public class Schedule {
	private long id = IDAO.ENTITY_UNSAVED;

	private Room room;
	private UserType userType;
	private DayOfWeek dayOfWeek;
	private Date beginTime;
	private Date endTime;

	private Calendar lastUpdate;
	private boolean active = true;

	@Id	@GeneratedValue(strategy = GenerationType.IDENTITY)
	public long getId() {return id;}
	public void setId(long id) {this.id = id;}

	@ManyToOne @JoinColumn(name = "roomId")	@ForeignKey(name = "roomId")
	public Room getRoom() {return room;}
	public void setRoom(Room room) {this.room = room;}

	@Enumerated(EnumType.STRING)
	@Column(length=UserType.COLUMN_DISCRIMINATOR_LENGTH)
	public UserType getUserType() {return userType;}
	public void setUserType(UserType userType) {this.userType = userType;}

	@Enumerated(EnumType.STRING)
	@Column(length=DayOfWeek.COLUMN_DISCRIMINATOR_LENGTH)
	public DayOfWeek getDayOfWeek() {return dayOfWeek;}
	public void setDayOfWeek(DayOfWeek dayOfWeek) {this.dayOfWeek = dayOfWeek;}

	@Column	@Temporal(TemporalType.TIME)
	public Date getBeginTime() {return beginTime;}
	public void setBeginTime(Date beginTime) {this.beginTime = beginTime;}

	@Column	@Temporal(TemporalType.TIME)
	public Date getEndTime() {return endTime;}
	public void setEndTime(Date endTime) {this.endTime = endTime;}

	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getLastUpdate() {return lastUpdate;}
	public void setLastUpdate(Calendar lastUpdate) {this.lastUpdate = lastUpdate;}

	@Column
	public boolean isActive() {return active;}
	public void setActive(boolean active) {this.active = active;}

	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(room != null?room.toString():"").append("(");
		sb.append(dayOfWeek != null?dayOfWeek.toString():"").append(" - ");
		sb.append(userType!=null?userType.toString():"").append(") [").append(CalendarUtils.formatTime(beginTime)).append("-").append(CalendarUtils.formatTime(endTime)).append("]");
		return sb.toString();
	}
}
