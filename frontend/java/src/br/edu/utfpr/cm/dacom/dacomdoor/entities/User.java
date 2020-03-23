package br.edu.utfpr.cm.dacom.dacomdoor.entities;

import java.util.Calendar;
import java.util.LinkedHashSet;
import java.util.Set;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
import javax.persistence.OrderBy;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;

import org.hibernate.annotations.ForeignKey;
import org.hibernate.annotations.LazyCollection;
import org.hibernate.annotations.LazyCollectionOption;

import br.com.orionsoft.monstrengo.core.util.StringUtils;
import br.com.orionsoft.monstrengo.crud.entity.dao.IDAO;

@Entity
@Table(name="users")
public class User {
	public static final String NAME = "name";
	public static final String E_MAIL = "email";
	public static final String UID = "uid";
	public static final String USER_TYPE = "userType";
	public static final String LAST_UPDATE = "lastUpdate";
	public static final String ACTIVE = "active";
	
	public static final String ROOMS_USER= "roomsUser";
	
	private long id = IDAO.ENTITY_UNSAVED;

	private String name;
	private String email;
	private String uid;

	private UserType userType;

	private Calendar lastUpdate;
	private boolean active = true;

	private Set<RoomUser> roomsUser = new LinkedHashSet<RoomUser>();
	
	@Id	@GeneratedValue(strategy = GenerationType.IDENTITY)
	public long getId() {return id;}
	public void setId(long id) {this.id = id;}
	
	@Column(length=80)
	public String getName() {return name;}
	public void setName(String name) {this.name = name;}
	
	@Column(length=100)
	public String getEmail() {return email;}
	public void setEmail(String email) {this.email = email;}
	
	@Column(length=16, unique=true)
	public String getUid() {return uid;}
	public void setUid(String uid) {this.uid = uid;}

	@Enumerated(EnumType.STRING)
	@Column(length=UserType.COLUMN_DISCRIMINATOR_LENGTH)
	public UserType getUserType() {return userType;}
	public void setUserType(UserType userType) {this.userType = userType;}
	
	@Column	@Temporal(TemporalType.TIMESTAMP)
	public Calendar getLastUpdate() {return lastUpdate;}
	public void setLastUpdate(Calendar lastUpdate) {this.lastUpdate = lastUpdate;}
	
	@Column
	public boolean isActive() {return active;}
	public void setActive(boolean active) {this.active = active;}
	
	@Column
	public Long getDecUid() {return StringUtils.isNotBlank(uid)?Long.parseLong(uid, 16):null;}
	public void setDecUid(Long uid) {this.uid = uid==null?null:String.format("%08X", uid);}

	@Override
	public String toString() {
		return name;
	}
	
	@OneToMany(cascade=CascadeType.ALL, orphanRemoval=true)
	@ForeignKey(name="userId")
	@JoinColumn(name="userId")
	@LazyCollection(LazyCollectionOption.FALSE)
	@OrderBy("room")
	public Set<RoomUser> getRoomsUser() {return roomsUser;}
	public void setRoomsUser(Set<RoomUser> roomsUser) {this.roomsUser = roomsUser;}
	
	public static void main(String[] args) {
		User u = new User();
		u.setDecUid(576l);
//		u.setUid("AA123434AA");
		System.out.println(u.getUid());
		System.out.println(u.getDecUid());
		
	}
}
