#ifndef APP_DOORLOCK
#define APP_DOORLOCK
#include "commons.cpp"
#include "config.cpp"
#include "utils.cpp"

#include "sqlite-db.cpp"
#include "models.cpp"

class Doorlock
{
private:
  String *lastUserType = NULL;
  SqliteDB *db;

public:
  String *getLastUserType() { return this->lastUserType; }

  Doorlock(SqliteDB *db) : db(db) {}

  String SELECT_USER_TYPE_FROM_KEYRINGS_BY_UID(String uid)
  {
    String command = "SELECT userType FROM keyrings WHERE uid = '$1' LIMIT 1;";
    command.replace("$1", uid);
    return command;
  }

  // The 'CURRENT' means in the current moment on time
  String SELECT_CURRENT_SCHEDULE_BY_USER_TYPE(String userType)
  {
    String now = Utils::currentDatetime();
    String dayOfWeek = Utils::currentWeekDay();

    String command = "SELECT * FROM schedules\nWHERE userType = '$1', dayOfWeek = '$2', beginTime <= '$3', endTime >= '$4'\nLIMIT 1";
    command.replace("$1", userType);
    command.replace("$2", dayOfWeek);
    command.replace("$3", now);
    command.replace("$4", now);
    return command;
  }

  void saveEvent(String uid, String eventType, String time)
  {
    EventsModel eventsModel;

    eventsModel.build(
        uid.c_str(),
        time,
        eventType.c_str());

    this->db->exec_(eventsModel.add().c_str());
  }

  void saveKeyring(String uid)
  {
    KeyringsModel keyringsModel;

    keyringsModel.build(
        uid.c_str(),             // uid
        -1,                      // userId
        "PROFESSOR",             // userType
        Utils::currentDatetime() // lastUpdate
    );

    this->db->exec(keyringsModel.add().c_str());
  }

  bool hasSchedules()
  {
    this->db->exec_(SchedulesModel::SELECT_FIRST().c_str());
    return this->db->hasResult();
  }

  bool checkAccessType(String userType)
  {
    Serial.println(F("[LOG]: Check if user type is allowed on current schedule."));

    bool isNotStudent = !userType.equals(UserTypeEnum::STUDENT);

    if (isNotStudent)
    {
      return true;
    }
    else if (this->hasSchedules())
    {
      this->db->exec_(this->SELECT_CURRENT_SCHEDULE_BY_USER_TYPE(userType).c_str());
      bool userInSchedule = this->db->hasResult();

      return userInSchedule;
    }

    return true;
  }

  bool checkSchedule(String uid)
  {
    Serial.println(F("[LOG]: Checking if uid exists on keyring."));

    this->db->exec1_(this->SELECT_USER_TYPE_FROM_KEYRINGS_BY_UID(uid).c_str());
    bool userExistInKeyring = this->db->hasResult();

    if (userExistInKeyring)
    {
      String userType = first_row.getCol("userType");
      lastUserType = &userType;

      return this->checkAccessType(userType);
    }
    else
    {
      return false;
    }
  }

  bool learnUid(String uid)
  {
    Serial.println(F("[LOG]: Learning uid."));
    this->db->exec_(this->SELECT_USER_TYPE_FROM_KEYRINGS_BY_UID(uid).c_str());

    if (this->db->hasResult())
    {
      return false;
    }
    else
    {
      this->saveKeyring(uid);
      return true;
    }
  }

  bool checkAccess(String uid)
  {
    bool hasAccess = this->checkSchedule(uid);

    if (hasAccess)
    {
      Serial.print(F("[LOG]: UID '"));
      Serial.print(uid);
      Serial.println(F("' allowed!"));
      return true;
    }
    else
    {
      Serial.print(F("[LOG]: UID '"));
      Serial.print(uid);
      Serial.println(F("' not allowed!"));
      return false;
    }
  }
};

#endif