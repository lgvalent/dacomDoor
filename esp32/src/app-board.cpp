#ifndef APP_BOARD
#define APP_BOARD
#include <ctime>

#include "utils.cpp"
#include "board-models.cpp"
#include "doorlock.cpp"
#include "reader.cpp"
#include "config.cpp"
class AppBoard
{
private:
  BoardModel *board;
  Doorlock *doorlock;
  AppConfig *appConfig;
  Reader *reader;

  Uid lastUid = 0;
  time_t lastUidTime = Utils::now();
  bool hasDoorOpenEvent;

public:
  time_t lastDoorOpenTime = 0;

  AppBoard(AppConfig *appConfig, BoardModel *board, Doorlock *doorlock, Reader *reader) : board(board),
                                                    doorlock(doorlock), appConfig(appConfig), reader(reader) {}

  void startup()
  {
  }

  void toggleDoor(Uid uid)
  {
    this->board->toggleLocked();
    bool isLocked = this->board->isLocked();

    if (this->appConfig->config.relayDelay > 1)
    {
      if (isLocked)
        this->board->lock();
      else
        this->board->unlock();
    }
    else
    {
      bool notIsLocked = !isLocked;
      if (notIsLocked)
        this->openDoor(uid);
    }

    if (this->appConfig->config.relayDelay > 1 || isLocked)
    {
      this->doorlock->saveEvent(
          uid,
          isLocked?EventType::OUT:EventType::IN,
          Utils::now());
    }

    for (int i = 0; i < 3; i++)
    {
      if (isLocked)
        this->board->beepOk();
      else
        this->board->beepNotOk();

      this->board->blinkActivityLed();
    }
  }

  void openDoor(Uid uid)
  {
    uint32_t t = this->appConfig->config.relayDelay * 1000.0;
    if (this->board->isLocked() || t < 1000)
    {
      this->board->unlock();
      delay(t);
      this->board->lock();
    }
    else
    {
      this->board->beepOk();
    }
  }

  void testDoorOpened()
  {
    bool isUnlocked = !this->board->isLocked();
    if (isUnlocked)
    {
      this->hasDoorOpenEvent = false;
    }
    else if (this->lastUid)
    {
      time_t t = time(NULL);
      time_t diff = t - this->lastDoorOpenTime; // seconds
      if (diff > this->appConfig->config.doorOpenedAlertDelay)
      {
        this->board->beepOk();
        this->board->blinkActivityLed();
        bool notHasDoorOpenEvent = !this->hasDoorOpenEvent;
        if (notHasDoorOpenEvent)
        {
          this->hasDoorOpenEvent = true;
          this->doorlock->saveEvent(
            this->lastUid,
            EventType::OPENED,
            Utils::now());
        }
      }
    }
  }

  void shutdownNow()
  {
    esp_deep_sleep(1000000000); // Sleep for 1000 seconds
  }

  void testShutdownNow()
  {
    if (this->board->isProgramButtonPushed() && this->board->isCommandButtonPushed())
    {
      Serial.println("[LOG]: Shutdown now");
      this->board->beepOk();
      this->board->beepNotOk();
      this->board->beepOk();
      this->shutdownNow();
    }
  }

  void blink()
  {
    this->board->blinkActivityLed();
    if (!this->board->isLocked())
    {
      this->board->blinkActivityLed();
    }
  }

  void tryLearnUid(Uid uid)
  {
    bool wasLearned = this->doorlock->learnUid(uid);

    if (wasLearned)
    {
      Serial.println(F("[LOG]: Learned"));
      this->board->beepOk();
    }
    else
    {
      Serial.println(F("[WARN]: Can't learn uid: uid already exists, or some error occurs!"));
      this->board->beepNotOk();
      this->board->beepNotOk();
    }
  }

  void tryCheckAccess(Uid uid)
  {
    bool hasAccess = this->doorlock->checkAccess(uid);

    if (hasAccess)
    {
      UserType userType = this->doorlock->getLastUserType();
      switch(userType){
        // Keep door opened when a professor open it
        case UserType::PROFESSOR: this->toggleDoor(uid); break;
        case UserType::EMPLOYEE: 
          // Allow employ closes an opened door
          if (this->board->isLocked())
            this->openDoor(uid);
          else
            this->toggleDoor(uid);
          break;
      }
    }
    else
    {
      this->board->beepNotOk();
    }
  }

  void readUid()
  {
    Uid uid = this->reader->readUid();
    if (uid == UID_NULL)
      return;

    this->lastUid = uid;
    this->lastUidTime = time(NULL);

    Serial.print("[LOG]: UID: ");
    Serial.println(uid);

    if (this->board->isProgramButtonPushed())
      this->tryLearnUid(uid);
    else
      this->tryCheckAccess(uid);
  }

  void run()
  {
    this->testDoorOpened();
    this->testShutdownNow();
    this->blink();
    this->readUid();
    delay(500);
  }
};

#endif