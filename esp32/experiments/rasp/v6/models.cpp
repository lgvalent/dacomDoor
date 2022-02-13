#ifndef APP_MODELS
#define APP_MODELS
#include <MFRC522.h>

class ModelBase
{
protected:
  int consumed = 0;
  int completed = 0;

public:
  void consume() { this->consumed = 0; }
  bool isCompleted() { return this->consumed == this->completed; }

  virtual String toJSON() = 0;

  virtual String add() = 0;
  virtual String remove() = 0;
  virtual String update() = 0;
  virtual String getById() = 0;
};

class EventsModel : public ModelBase
{
private:
  char uid[17];
  String time;       // datatime
  char eventType[5]; // "IN" | "OUT"
public:
  static String JSON_TEMPLATE() { return "{\"uid\":\"$1\",\"time\":\"$2\",\"eventType\":\"$3\"}"; }
  static String INSERT_TEMPLATE() { return "INSERT INTO events VALUES ('$1', '$2', '$3');"; }
  static String DELETE_TEMPLATE() { return "DELETE FROM events\nWHERE uid = '$1';"; }
  static String UPDATE_TEMPLATE()
  {
    return "UPDATE events\n"
           "SET time = '$2', eventType = '$3'\n"
           "WHERE uid = '$1';";
  }
  static String SELECT_BY_ID_TEMPLATE() { return "SELECT uid FROM events WHERE uid = $1 LIMIT 1;"; }

  EventsModel() { this->completed = 1 | 2 | 4; }

  void build(const char *uid, String time, const char *eventType)
  {
    this->setUid(uid);
    this->setTime(time);
    this->setEventType(eventType);
  }

  void setUid(const char *uid)
  {
    this->consumed |= 1;
    int i;
    for (i = 0; i < 16 && uid[i]; i++)
      this->uid[i] = uid[i];
    this->uid[i] = 0;
  }
  void setTime(String time)
  {
    this->consumed |= 2;
    this->time = time;
  }
  void setEventType(const char *eventType)
  {
    this->consumed |= 4;
    int i;
    for (i = 0; i < 4 && eventType[i]; i++)
      this->eventType[i] = eventType[i];
    this->eventType[i] = 0;
  }

  static String toJSON(char *uid, String time, char *eventType)
  {
    String json = EventsModel::JSON_TEMPLATE();
    json.replace("$2", String(uid));
    json.replace("$3", time);
    json.replace("$4", String(eventType));
    return json;
  }
  String toJSON() { return EventsModel::toJSON(this->uid, this->time, this->eventType); }

  String add()
  {
    String command = EventsModel::INSERT_TEMPLATE();
    command.replace("$1", String(this->uid));
    command.replace("$2", this->time);
    command.replace("$3", String(this->eventType));
    return command;
  }
  String remove()
  {
    String command = EventsModel::DELETE_TEMPLATE();
    command.replace("$1", String(this->uid));
    return command;
  }
  String update()
  {
    String command = EventsModel::UPDATE_TEMPLATE();
    command.replace("$1", String(this->uid));
    command.replace("$2", this->time);
    command.replace("$3", String(this->eventType));
    return command;
  }
  String getById()
  {
    String command = EventsModel::SELECT_BY_ID_TEMPLATE();
    command.replace("$1", String(this->uid));
    return command;
  }
};

class KeyringsModel : public ModelBase
{
private:
  long userId;
  char uid[17];
  char userType[11]; // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
  String lastUpdate; // datatime

public:
  static String JSON_TEMPLATE() { return "{\"userId\":$1,\"uid\":\"$2\",\"userType\":\"$3\",\"lastUpdate\":\"$4\"}"; }
  static String INSERT_TEMPLATE() { return "INSERT INTO keyrings VALUES ($1, '$2', '$3', '$4');"; }
  static String DELETE_TEMPLATE() { return "DELETE FROM keyrings WHERE userId = $1;"; }
  static String UPDATE_TEMPLATE()
  {
    return "UPDATE keyrings\n"
           "SET uid = '$2', userType = '$3', lastUpdate = '$4'\n"
           "WHERE userId = $1;";
  }
  static String SELECT_BY_ID_TEMPLATE() { return "SELECT userId FROM keyrings WHERE userId = $1 LIMIT 1;"; }

  KeyringsModel() { this->completed = 1 | 2 | 4 | 8; }

  void build(const char *uid, long userId, const char *userType, String lastUpdate)
  {
    this->setUserId(userId);
    this->setUid(uid);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setUserId(long userId)
  {
    this->consumed |= 1;
    this->userId = userId;
  }
  void setUid(const char *uid)
  {
    this->consumed |= 2;
    int i;
    for (i = 0; i < 16 && uid[i]; i++)
      this->uid[i] = uid[i];
    this->uid[i] = 0;
  }
  void setUserType(const char *userType)
  {
    this->consumed |= 4;
    int i;
    for (i = 0; i < 10 && userType[i]; i++)
      this->userType[i] = userType[i];
    this->userType[i] = 0;
  }
  void setLastUpdate(String lastUpdate)
  {
    this->consumed |= 8;
    this->lastUpdate = lastUpdate;
  }

  static String toJSON(int userId, char *uid, char *userType, String lastUpdate)
  {
    String json = KeyringsModel::JSON_TEMPLATE();
    json.replace("$1", String(userId));
    json.replace("$2", String(uid));
    json.replace("$3", String(userType));
    json.replace("$4", lastUpdate);
    return json;
  }
  String toJSON() { return KeyringsModel::toJSON(this->userId, this->uid, this->userType, this->lastUpdate); }

  String add()
  {
    String command = KeyringsModel::INSERT_TEMPLATE();
    command.replace("$1", String(this->userId));
    command.replace("$2", String(this->uid));
    command.replace("$3", String(this->userType));
    command.replace("$4", this->lastUpdate);
    return command;
  }
  String remove()
  {
    String command = KeyringsModel::DELETE_TEMPLATE();
    command.replace("$1", String(this->userId));
    return command;
  }
  String update()
  {
    String command = KeyringsModel::UPDATE_TEMPLATE();
    command.replace("$1", String(this->userId));
    command.replace("$2", String(this->uid));
    command.replace("$3", String(this->userType));
    command.replace("$4", this->lastUpdate);
    return command;
  }
  String getById()
  {
    String command = KeyringsModel::SELECT_BY_ID_TEMPLATE();
    command.replace("$1", String(this->userId));
    return command;
  }
};

class SchedulesModel : public ModelBase
{
private:
  long id;
  char dayOfWeek[11]; // "SUNDAY" | "MONDAY" | "THUESDAY" | "WEDNESDAY" | "THURSDAY" | "FRIDAY" | "SATURDAY"
  String beginTime;   // time
  String endTime;     // time
  char userType[11];  // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
  String lastUpdate;  // datatime
public:
  static String JSON_TEMPLATE() { return "{\"dayOfWeek\":\"$1\",\"beginTime\":\"$2\",\"endTime\":\"$3\",\"userType\":\"$4\",\"lastUpdate\":\"$5\"}"; }
  static String INSERT_TEMPLATE() { return "INSERT INTO schedules VALUES ($1, '$2', '$3', '$4', '$5', '$6');"; }
  static String DELETE_TEMPLATE() { return "DELETE FROM schedules WHERE id = $1;"; }
  static String UPDATE_TEMPLATE()
  {
    return "UPDATE schedules\n"
           "SET dayOfWeek = '$2', beginTime = '$3', endTime = '$4', userType = '$5', lastUpdate = '$6'\n"
           "WHERE id = $1;";
  }
  static String SELECT_BY_ID_TEMPLATE() { return "SELECT id FROM schedules WHERE id = $1 LIMIT 1;"; }

  static String SELECT_FIRST() { return "SELECT * FROM schedules LIMIT 1;"; }

  SchedulesModel() { this->completed = 1 | 2 | 4 | 8 | 16 | 32; }

  void build(long id, const char *dayOfWeek, String beginTime, String endTime, const char *userType, String lastUpdate)
  {
    this->setId(id);
    this->setDayOfWeek(dayOfWeek);
    this->setBeginTime(beginTime);
    this->setEndTime(endTime);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setId(long id)
  {
    this->consumed |= 1;
    this->id = id;
  }
  void setDayOfWeek(const char *dayOfWeek)
  {
    this->consumed |= 2;
    int i;
    for (i = 0; i < 10 && dayOfWeek[i]; i++)
      this->dayOfWeek[i] = dayOfWeek[i];
    this->dayOfWeek[i] = 0;
  }
  void setBeginTime(String beginTime)
  {
    this->consumed |= 4;
    this->beginTime = beginTime;
  }
  void setEndTime(String endTime)
  {
    this->consumed |= 8;
    this->endTime = endTime;
  }
  void setUserType(const char *userType)
  {
    this->consumed |= 16;
    int i;
    for (i = 0; i < 10 && userType[i]; i++)
      this->userType[i] = userType[i];
    this->userType[i] = 0;
  }
  void setLastUpdate(String lastUpdate)
  {
    this->consumed |= 32;
    this->lastUpdate = lastUpdate;
  }

  static String toJSON(long id, char *dayOfWeek, String beginTime, String endTime, char *userType, String lastUpdate)
  {
    String json = SchedulesModel::JSON_TEMPLATE();
    json.replace("$1", String(dayOfWeek));
    json.replace("$2", beginTime);
    json.replace("$3", endTime);
    json.replace("$4", String(userType));
    json.replace("$5", lastUpdate);
    return json;
  }
  String toJSON() { return SchedulesModel::toJSON(this->id, this->dayOfWeek, this->beginTime, this->endTime, this->userType, this->lastUpdate); }

  String add()
  {
    String command = SchedulesModel::INSERT_TEMPLATE();
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->dayOfWeek));
    command.replace("$3", this->beginTime);
    command.replace("$4", this->endTime);
    command.replace("$5", String(this->userType));
    command.replace("$6", this->lastUpdate);
    return command;
  }
  String remove()
  {
    String command = SchedulesModel::DELETE_TEMPLATE();
    command.replace("$1", String(this->id));
    return command;
  }
  String update()
  {
    String command = SchedulesModel::UPDATE_TEMPLATE();
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->dayOfWeek));
    command.replace("$3", this->beginTime);
    command.replace("$4", this->endTime);
    command.replace("$5", String(this->userType));
    command.replace("$6", this->lastUpdate);
    return command;
  }
  String getById()
  {
    String command = SchedulesModel::SELECT_BY_ID_TEMPLATE();
    command.replace("$1", String(this->id));
    return command;
  }
};

namespace UserTypeEnum
{
  static String STUDENT = "Student";
  static String PROFESSOR = "Professor";
  static String EMPLOYEE = "Employee";
};

#endif