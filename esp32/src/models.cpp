#ifndef APP_MODELS
#define APP_MODELS

#include <Arduino.h>
#include <unordered_map>
#include <ctime>
#include "utils.cpp"


#define HEX8(i) ([](uint32_t val) -> const char* { \
    static char buf[9]; \
    snprintf(buf, sizeof(buf), "%08X", val); \
    return buf; \
}(i))

class ModelBase
{
public:
  virtual bool isValid() const = 0;
  virtual String toJSON() const = 0;
};

typedef u_int32_t Uid; 

#define UID_NULL 0

enum class DayOfWeek: char{SUNDAY,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY};

static std::unordered_map<DayOfWeek, String> dayOfWeekNames = { 
  {DayOfWeek::SUNDAY, "SUNDAY"},
  {DayOfWeek::MONDAY, "MONDAY"},
  {DayOfWeek::TUESDAY, "TUESDAY"},
  {DayOfWeek::WEDNESDAY, "WEDNESDAY"},
  {DayOfWeek::THURSDAY, "THURSDAY"},
  {DayOfWeek::FRIDAY, "FRIDAY"},
  {DayOfWeek::SATURDAY, "SATURDAY"}}; 

enum class EventType: char{IN, OUT, DOOR_OPENED};

static std::unordered_map<EventType, String> eventTypeNames = { 
  {EventType::IN, "IN"},
  {EventType::OUT, "OUT"},
  {EventType::DOOR_OPENED, "DOOR_OPENED"}}; 
  
enum class UserType: char{STUDENT, PROFESSOR, EMPLOYEE};

static std::unordered_map<UserType, String> userTypeNames = { 
  {UserType::STUDENT, "STUDENT"},
  {UserType::PROFESSOR, "PROFESSOR"},
  {UserType::EMPLOYEE, "EMPLOYEE"}}; 

class Event: public ModelBase
{
private:
  Uid uid = 0;
  time_t dateTime;
  EventType eventType;
public:
  static String JSON_TEMPLATE() { return "{\"uid\":\"$1\",\"dateTime\":\"$2\",\"eventType\":\"$3\"}"; }

  Event() { }

  bool isValid()const{return this->uid!=0;}

  void build(const Uid uid, time_t dateTime, EventType eventType)
  {
    this->setUid(uid);
    this->setTime(dateTime);
    this->setEventType(eventType);
  }

  Uid getUid()const {return this->uid;}
  void setUid(const Uid uid)
  {
    this->uid = uid;
  }
  time_t getTime()const{return this->dateTime;}
  void setTime(time_t dateTime)
  {
    this->dateTime = dateTime;
  }
  EventType getEventType()const{return this->eventType;}
  void setEventType(EventType eventType)
  {
    this->eventType = eventType;
  }

  static String toJSON(Uid uid, time_t dateTime, EventType eventType)
  {
    String json = Event::JSON_TEMPLATE();
    json.replace("$1", String(HEX8(uid)));
    json.replace("$2", Utils::datetimeToString(dateTime));
    json.replace("$3", eventTypeNames[eventType]);
    return json;
  }

  String toJSON() const{ return Event::toJSON(this->uid, this->dateTime, this->eventType); }
};

class Keyring : public ModelBase
{
private:
  Uid userId;
  Uid uid = 0;
  UserType userType;
  time_t lastUpdate;

public:
  static String JSON_TEMPLATE() { return "{\"userId\":$1,\"uid\":$2,\"userType\":\"$3\",\"lastUpdate\":\"$4\"}"; }

  Keyring() {}

  bool isValid()const{return this->uid!=0;}

  void build(const Uid uid,const Uid userId, const UserType userType, const time_t lastUpdate)
  {
    this->setUserId(userId);
    this->setUid(uid);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setUserId(Uid userId)
  {
    this->userId = userId;
  }
  void setUid(const Uid uid)
  {
    this->uid = uid;
  }
  void setUserType(const UserType userType)
  {
    this->userType = userType;
  }
  void setLastUpdate(time_t lastUpdate)
  {
    this->lastUpdate = lastUpdate;
  }

  Uid getUserId() const
  {
    return this->userId;
  }

  Uid getUid() const
  {
    return this->uid;
  }

  UserType getUserType() const
  {
    return this->userType;
  }

  time_t getLastUpdate() const
  {
    return this->lastUpdate;
  }

  static String toJSON(const Uid userId, const Uid uid, UserType userType, time_t lastUpdate)
  {
    String json = Keyring::JSON_TEMPLATE();
    json.replace("$1", String(userId));
    json.replace("$2", String(HEX8(uid)));
    json.replace("$3", userTypeNames[userType]);
    json.replace("$4", Utils::datetimeToString(lastUpdate));
    return json;
  }
  String toJSON() const { return Keyring::toJSON(this->userId, this->uid, this->userType, this->lastUpdate); }
};


class Schedule : public ModelBase
{
private:
  Uid id = 0;
  DayOfWeek dayOfWeek;
  time_t beginTime;   // time
  time_t endTime;     // time
  UserType userType;
  time_t lastUpdate;  // datatime
public:
  static String JSON_TEMPLATE() { return "{\"dayOfWeek\":\"$1\",\"beginTime\":\"$2\",\"endTime\":\"$3\",\"userType\":\"$4\",\"lastUpdate\":\"$5\"}"; }

  Schedule() {}
 
  bool isValid()const{return this->id!=0;}

  void build(Uid id, const DayOfWeek dayOfWeek, time_t beginTime, time_t endTime, const UserType userType, time_t lastUpdate)
  {
    this->setId(id);
    this->setDayOfWeek(dayOfWeek);
    this->setBeginTime(beginTime);
    this->setEndTime(endTime);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setId(Uid id)
  {
    this->id = id;
  }
  void setDayOfWeek(const DayOfWeek dayOfWeek)
  {
    this->dayOfWeek = dayOfWeek;
  }
  void setBeginTime(time_t beginTime)
  {
    this->beginTime = beginTime;
  }
  void setEndTime(time_t endTime)
  {
    this->endTime = endTime;
  }
  void setUserType(const UserType userType)
  {
    this->userType = userType;
  }
  void setLastUpdate(time_t lastUpdate)
  {
    this->lastUpdate = lastUpdate;
  }

  Uid getId() const
  {
    return this->id;
  }

  DayOfWeek getDayOfWeek() const
  {
    return this->dayOfWeek;
  }

  time_t getBeginTime() const
  {
    return this->beginTime;
  }

  time_t getEndTime() const
  {
    return this->endTime;
  }

  UserType getUserType() const
  {
    return this->userType;
  }

  time_t getLastUpdate() const
  {
    return this->lastUpdate;
  }

  static String toJSON(Uid id, const DayOfWeek dayOfWeek, time_t beginTime, time_t endTime, const UserType userType, time_t lastUpdate) 
  {
    String json = Schedule::JSON_TEMPLATE();
    json.replace("$1", Utils::weekDay(static_cast<char>(dayOfWeek)));
    json.replace("$2", Utils::datetimeToString(beginTime));
    json.replace("$3", Utils::datetimeToString(endTime));
    json.replace("$4", userTypeNames[userType]);
    json.replace("$5", Utils::datetimeToString(lastUpdate));
    return json;
  }
  String toJSON() const { return Schedule::toJSON(this->id, this->dayOfWeek, this->beginTime, this->endTime, this->userType, this->lastUpdate); }
};

class Config {
  public:
  inline static String CONFIG_PASSWORD =  "CONFIG_PASSWORD";
  inline static String BOARD_VERSION =  "BOARD_VERSION";
  inline static String ROOM_NAME =  "ROOM_NAME";
  inline static String LAST_UPDATE =  "LAST_UPDATE";
  inline static String RELAY_DELAY =  "RELAY_DELAY";
  inline static String WIFI_SSID =  "WIFI_SSID";
  inline static String WIFI_PASSWORD =  "WIFI_PASSWORD";
  inline static String GMT_ZONE =  "GMT_ZONE";
  inline static String NEW_CONFIG_PASSWORD =  "NEW_CONFIG_PASSWORD*";
  inline static String SERVER_URL =  "SERVER_URL*";
  inline static String UPDATE_DELAY =  "UPDATE_DELAY*";
  inline static String DOOR_OPENED_ALERT_DELAY =  "DOOR_OPENED_ALERT_DELAY*";

  inline static int MAX_RELAY_DELAY_FOR_MAGNETIC =  1000;

  int boardVersion = 1;
  String configPassword = "admin";
  String serverURL = "http://dacom.cm.utfpr.edu.br:19500";
  String roomName = "CAFE";
  time_t lastUpdate = 0;
  int updateDelay = 300;                  // seconds
  int relayDelay = 100;                  // miliseconds
  int doorOpenedAlertDelay = 30;         // seconds
  String wifiSSID = "UTFPR-IOT";
  String wifiPassword = "";
  int gmtZone = -3;                      // GMT-3 Brasília
  String newConfigPassword = "";

  static String JSON_TEMPLATE() {
    return "{\"boardVersion\":\"$1\",\"configPassword\":\"$2\",\"serverURL\":\"$3\",\"roomName\":\"$4\",\"lastUpdate\":$5,\"updateDelay\":$6,\"relayDelay\":$7,\"doorOpenedAlertDelay\":$8,\"wifiSSID\":\"$9\",\"wifiPassword\":\"$10\",\"gmtZone\":$11}";
  }
  
  void applyConfig(const String& key, const String& value) {
   if (key == Config::CONFIG_PASSWORD) this->configPassword = value;
   else if (key == Config::BOARD_VERSION) this->boardVersion = value.toInt();
   else if (key == Config::SERVER_URL) this->serverURL = value;
   else if (key == Config::ROOM_NAME) this->roomName = value;
   else if (key == Config::LAST_UPDATE) this->lastUpdate = value.toInt();
   else if (key == Config::UPDATE_DELAY) this->updateDelay = value.toInt();
   else if (key == Config::RELAY_DELAY) this->relayDelay = value.toInt();
   else if (key == Config::DOOR_OPENED_ALERT_DELAY) this->doorOpenedAlertDelay = value.toInt();
   else if (key == Config::WIFI_SSID) this->wifiSSID = value;
   else if (key == Config::WIFI_PASSWORD) this->wifiPassword = value;
   else if (key == Config::GMT_ZONE) this->gmtZone = value.toInt();
   else if (key == Config::NEW_CONFIG_PASSWORD) this->newConfigPassword = value;
   else {
	  Serial.printf("[ERROR] Unknown config key: %s\n", key.c_str());
   }
 }
  static String toJSON(const Config *config) {
    String json = Config::JSON_TEMPLATE();
    json.replace("$1", String(config->boardVersion));
    json.replace("$2", config->configPassword);
    json.replace("$3", config->serverURL);
    json.replace("$4", config->roomName);
    json.replace("$5", String(config->lastUpdate));
    json.replace("$6", String(config->updateDelay));
    json.replace("$7", String(config->relayDelay));
    json.replace("$8", String(config->doorOpenedAlertDelay));
    json.replace("$9", config->wifiSSID);
    json.replace("$10", config->wifiPassword);
    json.replace("$11", String(config->gmtZone));
    return json;
  }
  String toJSON() const { return Config::toJSON(this); }
};


#endif