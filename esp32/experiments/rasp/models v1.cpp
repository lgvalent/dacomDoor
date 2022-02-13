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

  String addSafe()
  {
    if (this->isCompleted())
    {
      this->consumed = 0;
      return this->add();
    }
    return "";
  }
  String removeSafe()
  {
    if ((this->consumed & 1) == 1)
    {
      this->consumed = 0;
      return this->remove();
    }
    return "";
  }
  String updateSafe()
  {
    if (this->isCompleted())
    {
      this->consumed = 0;
      return this->update();
    }
    return "";
  }
};

class EventsModel : public ModelBase
{
private:
  int id;
  char uid[17];
  String time;       // datatime
  char eventType[5]; // "IN" | "OUT"
public:
  const static String JSON_TEMPLATE;
  const static String INSERT_TEMPLATE;
  const static String DELETE_TEMPLATE;
  const static String UPDATE_TEMPLATE;

  EventsModel() { this->completed = 1 | 2 | 4 | 8; }

  void build(int id, char *uid, String time, char *eventType)
  {
    this->setId(id);
    this->setUid(uid);
    this->setTime(time);
    this->setEventType(eventType);
  }

  void setId(int id)
  {
    this->consumed |= 1;
    this->id = id;
  }
  void setUid(const char *uid)
  {
    this->consumed |= 2;
    int i;
    for (i = 0; i < 16 && uid[i]; i++)
      this->uid[i] = uid[i];
    this->uid[i] = 0;
  }
  void setTime(String time)
  {
    this->consumed |= 4;
    this->time = time;
  }
  void setEventType(const char *eventType)
  {
    this->consumed |= 8;
    int i;
    for (i = 0; i < 4 && eventType[i]; i++)
      this->eventType[i] = eventType[i];
    this->eventType[i] = 0;
  }

  static String toJSON(int id, char *uid, String time, char *eventType)
  {
    String json = EventsModel::JSON_TEMPLATE;
    json.replace("$1", String(id));
    json.replace("$2", String(uid));
    json.replace("$3", time);
    json.replace("$4", String(eventType));
    return json;
  }
  String toJSON() { return EventsModel::toJSON(this->id, this->uid, this->time, this->eventType); }

  String add()
  {
    String command = EventsModel::INSERT_TEMPLATE;
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->uid));
    command.replace("$3", this->time);
    command.replace("$4", String(this->eventType));
    return command;
  }
  String remove()
  {
    String command = EventsModel::DELETE_TEMPLATE;
    command.replace("$1", String(this->id));
    return command;
  }
  String update()
  {
    String command = EventsModel::UPDATE_TEMPLATE;
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->uid));
    command.replace("$3", this->time);
    command.replace("$4", String(this->eventType));
    return command;
  }
};

const String EventsModel::JSON_TEMPLATE = "{\"id\":$1,\"uid\":\"$2\",\"time\":\"$3\",\"eventType\":\"$4\"}";
const String EventsModel::INSERT_TEMPLATE = "INSERT INTO events VALUES ($1, '$2', '$3', '$4');";
const String EventsModel::DELETE_TEMPLATE = "DELETE FROM events\nWHERE id = $1;";
const String EventsModel::UPDATE_TEMPLATE = "UPDATE events\n"
                                            "SET uid = '$2', time = '$3', eventType = '$4'\n"
                                            "WHERE id = $1;";

class KeyringsModel : public ModelBase
{
private:
  int id;
  char uid[17];
  int userId;
  char userType[11]; // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
  String lastUpdate; // datatime

public:
  const static String JSON_TEMPLATE;
  const static String INSERT_TEMPLATE;
  const static String DELETE_TEMPLATE;
  const static String UPDATE_TEMPLATE;

  KeyringsModel() { this->completed = 1 | 2 | 4 | 8 | 16; }

  void build(int id, char *uid, int userId, char *userType, String lastUpdate)
  {
    this->setId(id);
    this->setUid(uid);
    this->setUserId(userId);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setId(int id)
  {
    this->consumed |= 1;
    this->id = id;
  }
  void setUid(const char *uid)
  {
    this->consumed |= 2;
    int i;
    for (i = 0; i < 16 && uid[i]; i++)
      this->uid[i] = uid[i];
    this->uid[i] = 0;
  }
  void setUserId(int userId)
  {
    this->consumed |= 4;
    this->userId = userId;
  }
  void setUserType(const char *userType)
  {
    this->consumed |= 8;
    int i;
    for (i = 0; i < 10 && userType[i]; i++)
      this->userType[i] = userType[i];
    this->userType[i] = 0;
  }
  void setLastUpdate(String lastUpdate)
  {
    this->consumed |= 16;
    this->lastUpdate = lastUpdate;
  }

  static String toJSON(int id, char *uid, int userId, char *userType, String lastUpdate)
  {
    String json = KeyringsModel::JSON_TEMPLATE;
    json.replace("$1", String(id));
    json.replace("$2", String(uid));
    json.replace("$3", String(userId));
    json.replace("$4", String(userType));
    json.replace("$5", lastUpdate);
    return json;
  }
  String toJSON() { return KeyringsModel::toJSON(this->id, this->uid, this->userId, this->userType, this->lastUpdate); }

  String add()
  {
    String command = KeyringsModel::INSERT_TEMPLATE;
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->uid));
    command.replace("$3", String(this->userId));
    command.replace("$4", String(this->userType));
    command.replace("$5", this->lastUpdate);
    return command;
  }
  String remove()
  {
    String command = KeyringsModel::DELETE_TEMPLATE;
    command.replace("$1", String(this->id));
    return command;
  }
  String update()
  {
    String command = KeyringsModel::UPDATE_TEMPLATE;
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->uid));
    command.replace("$3", String(this->userId));
    command.replace("$4", String(this->userType));
    command.replace("$5", this->lastUpdate);
    return command;
  }
};

const String KeyringsModel::JSON_TEMPLATE = "{\"id\":$1,\"uid\":\"$2\",\"userId\":$3,\"userType\":\"$4\",\"lastUpdate\":\"$5\"}";
const String KeyringsModel::INSERT_TEMPLATE = "INSERT INTO keyrings VALUES ($1, '$2', $3, '$4', '$5');";
const String KeyringsModel::DELETE_TEMPLATE = "DELETE FROM keyrings WHERE id = $1;";
const String KeyringsModel::UPDATE_TEMPLATE = "UPDATE keyrings\n"
                                              "SET uid = '$2', userId = $3, userType = '$4', lastUpdate = '$5'\n"
                                              "WHERE id = $1;";

class SchedulesModel : public ModelBase
{
private:
  int id;
  char dayOfWeek[11]; // "SUNDAY" | "MONDAY" | "THUESDAY" | "WEDNESDAY" | "THURSDAY" | "FRIDAY" | "SATURDAY"
  String beginTime;   // time
  String endTime;     // time
  char userType[11];  // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
  String lastUpdate;  // datatime
public:
  const static String JSON_TEMPLATE;
  const static String INSERT_TEMPLATE;
  const static String DELETE_TEMPLATE;
  const static String UPDATE_TEMPLATE;

  SchedulesModel() { this->completed = 1 | 2 | 4 | 8 | 16 | 32; }

  void build(int id, char *dayOfWeek, String beginTime, String endTime, char *userType, String lastUpdate)
  {
    this->setId(id);
    this->setDayOfWeek(dayOfWeek);
    this->setBeginTime(beginTime);
    this->setEndTime(endTime);
    this->setUserType(userType);
    this->setLastUpdate(lastUpdate);
  }

  void setId(int id)
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

  static String toJSON(int id, char *dayOfWeek, String beginTime, String endTime, char *userType, String lastUpdate)
  {
    String json = SchedulesModel::JSON_TEMPLATE;
    json.replace("$1", String(id));
    json.replace("$2", String(dayOfWeek));
    json.replace("$3", beginTime);
    json.replace("$4", endTime);
    json.replace("$5", String(userType));
    json.replace("$6", lastUpdate);
    return json;
  }
  String toJSON() { return SchedulesModel::toJSON(this->id, this->dayOfWeek, this->beginTime, this->endTime, this->userType, this->lastUpdate); }

  String add()
  {
    String command = SchedulesModel::INSERT_TEMPLATE;
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
    String command = SchedulesModel::DELETE_TEMPLATE;
    command.replace("$1", String(this->id));
    return command;
  }
  String update()
  {
    String command = SchedulesModel::UPDATE_TEMPLATE;
    command.replace("$1", String(this->id));
    command.replace("$2", String(this->dayOfWeek));
    command.replace("$3", this->beginTime);
    command.replace("$4", this->endTime);
    command.replace("$5", String(this->userType));
    command.replace("$6", this->lastUpdate);
    return command;
  }
};

const String SchedulesModel::JSON_TEMPLATE = "{\"id\":$1,\"dayOfWeek\":\"$2\",\"beginTime\":\"$3\",\"endTime\":\"$4\",\"userType\":\"$5\",\"lastUpdate\":\"$6\"}";
const String SchedulesModel::INSERT_TEMPLATE = "INSERT INTO schedules VALUES ($1, '$2', '$3', '$4', '$5', '$6');";
const String SchedulesModel::DELETE_TEMPLATE = "DELETE FROM schedules WHERE id = $1;";
const String SchedulesModel::UPDATE_TEMPLATE = "UPDATE schedules\n"
                                               "SET dayOfWeek = '$2', beginTime = '$3', endTime = '$4', userType = '$5', lastUpdate = '$6'\n"
                                               "WHERE id = $1;";

void setup()
{
  // Wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);

  EventsModel e;
  e.setId(200);
  e.setUid("44:66:A5:E2:29");
  e.setTime("2021-02-02 15:00:00");
  e.setEventType("IN");

  Serial.println("");
  Serial.println(e.isCompleted());
  Serial.println(e.add());
  Serial.println(e.remove());
  Serial.println(e.update());
  Serial.println(e.toJSON());

  e.setId(1000);
  e.setUid("44:66:A5:E2:29");
  e.setTime("2021-02-02 15:00:00");
  e.setEventType("OUT");

  Serial.println("");
  Serial.println(e.isCompleted());
  Serial.println(e.add());
  Serial.println(e.remove());
  Serial.println(e.update());
  Serial.println(e.toJSON());

  KeyringsModel k;
  k.setId(300);
  k.setUid("84:62:A2:EF:30");
  k.setUserId(250);
  k.setUserType("PROFESSOR");
  k.setLastUpdate("2021-02-02 06:00:00");

  Serial.println("");
  Serial.println(k.isCompleted());
  Serial.println(k.add());
  Serial.println(k.remove());
  Serial.println(k.update());
  Serial.println(k.toJSON());

  SchedulesModel s;
  s.setId(100);
  s.setDayOfWeek("SUNDAY");
  s.setBeginTime("12:00:00");
  s.setEndTime("18:00:00");
  s.setUserType("PROFESSOR");
  s.setLastUpdate("2021-02-02 12:00:00");

  Serial.println("");
  Serial.println(s.isCompleted());
  Serial.println(s.add());
  Serial.println(s.remove());
  Serial.println(s.update());
  Serial.println(s.toJSON());
}

void loop()
{
}
