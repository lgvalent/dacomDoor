#ifndef APP_DOORLOCK
#define APP_DOORLOCK
#include <MFRC522.h>
#include <Time.h>

#include "utils.cpp"
#include "app.cpp"

class Doorlock
{
private:
  AppDb *app;

public:
  Doorlock(AppDb *app) : app(app) {}

  String uidExistsInKeyring(String uid)
  {
    String command = "SELECT userType FROM keyrings WHERE uid = '$1' LIMIT 1;";
    command.replace("$1", uid);
    return command;
  }

  String userTypeInSchedule(String userType) {
    String now = Utils::currentDatetime();
    String dayOfWeek = Utils::currentWeekDay();

    String command = "SELECT * FROM schedules\nWHERE userType = '$1', dayOfWeek = '$2', beginTime <= '$3', endTime >= '$4'\nLIMIT 1";
    command.replace("$1", userType);
    command.replace("$2", dayOfWeek);
    command.replace("$3", now);
    command.replace("$4", now);
    return command;
  }

  bool saveEvent(String uid, String eventType, String time)
  {
    EventsModel eventsModel;

    eventsModel.build(
        uid.c_str(),
        time,
        eventType.c_str());

    this->app->exec_(eventsModel.add().c_str());

    return this->app->hasResult();
  }

  bool hasSchedules()
  {
    this->app->exec_(SchedulesModel::SELECT_FIRST().c_str());
    return this->app->hasResult();
  }

  bool checkAccessType(String userType)
  {
    Serial.println(F("[LOG]: Check if user type is allowed on schedule."));
    bool isNotStudent = !userType.equals(UserTypeEnum::STUDENT);

    if (isNotStudent)
    {
      return true;
    }
    else if (this->hasSchedules())
    {
      this->app->exec_(this->userTypeInSchedule(userType).c_str());
      bool userInSchedule = this->app->hasResult();
      return userInSchedule;
    }

    return true;
  }

  bool checkSchedule(String uid)
  {
    Serial.println(F("[LOG]: Check if uid exists on keyring."));
    this->app->exec1_(this->uidExistsInKeyring(uid).c_str());

    if (this->app->hasResult())
    {
      String userType = first_row.getCol("userType");

      return this->checkAccessType(userType);
    }
    else
    {
      return false;
    }
  }

  bool learnUid(String uid)
  {
    this->app->exec_(this->uidExistsInKeyring(uid).c_str());

    if (this->app->hasResult())
    {
      return false;
    }
    else
    {
      KeyringsModel keyringsModel;

      keyringsModel.build(
          uid.c_str(),            // uid
          -1,                     // userId
          "PROFESSOR",            // userType
          Utils::currentDatetime() // lastUpdate
      );

      this->app->exec(keyringsModel.add().c_str());

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