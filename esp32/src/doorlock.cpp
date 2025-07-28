#ifndef APP_DOORLOCK
#define APP_DOORLOCK
#include "config.cpp"
#include "utils.cpp"

#include "models.cpp"
#include "daosqlite3.cpp"

class Doorlock
{
private:
  UserType lastUserType;
  DaoManager *daoManager;
public:
  UserType getLastUserType() { return this->lastUserType; }

  Doorlock():daoManager(&DaoManager::instance()){
    Keyring keyring;
    keyring.build(
        5657481,                     // uid
        123456,                     // userId
        UserType::PROFESSOR,   // userType
        Utils::now()           // lastUpdate
    );

    this->daoManager->keyringDao.save(keyring);
  }

  void saveEvent(const Uid uid,const EventType eventType, const time_t time)
  {
    Event event;

    event.build(
        uid,
        time,
        eventType);
    this->daoManager->eventDao.save(event);
  }

  void saveKeyring(Uid uid)
  {
    Keyring keyring;

    keyring.build(
        uid,                     // uid
        0,                       // userId
        UserType::PROFESSOR,     // userType
        Utils::now() // lastUpdate
    );

    daoManager->keyringDao.save(keyring);
  }

  bool checkAccessType(UserType userType, time_t time)
  {
    Serial.println(F("[LOG]: Check if user type is allowed on current schedule."));

    bool isNotStudent = userType != UserType::STUDENT;

    return userType != UserType::STUDENT || daoManager->scheduleDao.hasSchedule(userType, time);
  }

  bool checkSchedule(Uid uid)
  {
    Serial.println(F("[LOG]: Checking if uid exists on keyring."));

    Keyring keyring = daoManager->keyringDao.findByUid(uid);

    if (keyring.isValid())
    {      
      bool result = this->checkAccessType(keyring.getUserType(), Utils::now());
      if(result)lastUserType = keyring.getUserType();
      return result;
    }
    else
    {
      return false;
    }
  }

  bool learnUid(Uid uid)
  {
    Serial.println(F("[LOG]: Learning uid."));
    Keyring keyring = daoManager->keyringDao.findByUid(uid);
    if (keyring.isValid())
    {
      return false;
    }
    else
    {
      this->saveKeyring(uid);
      return true;
    }
  }

  bool checkAccess(Uid uid)
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