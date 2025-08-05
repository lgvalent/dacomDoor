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
public:
  bool unlocked = false;
  bool hasDoorOpenEvent = false;
  time_t lastDoorOpenTime = 0;

  AppBoard(AppConfig *appConfig, BoardModel *board, Doorlock *doorlock, Reader *reader) : board(board),
                                                    doorlock(doorlock), appConfig(appConfig), reader(reader) {}

  void startup()
  {
  }

  void toggleDoor(Uid uid)
  {
    this->unlocked = !this->unlocked;
    if (this->appConfig->config.relayDelay > Config::MAX_RELAY_DELAY_FOR_MAGNETIC)
    {
      if (this->unlocked)
        this->board->unlock();
      else
        this->board->lock();
    }
    else
    {
      if (this->unlocked){
        this->openDoor(uid);
      }
    }

    // Save the last door open(IN)/close(OUT) time
    if (!this->unlocked || this->appConfig->config.relayDelay > Config::MAX_RELAY_DELAY_FOR_MAGNETIC)
    {
      this->doorlock->saveEvent(
          uid,
          this->unlocked?EventType::IN:EventType::OUT,
          Utils::now());
    }

    for (int i = 0; i < 3; i++)
    {
      if (this->unlocked)
        this->board->beepOk();
      else
        this->board->beepNotOk();
    }
  }

  void openDoor(Uid uid)
  {
    if (this->board->isLocked() || this->appConfig->config.relayDelay <= Config::MAX_RELAY_DELAY_FOR_MAGNETIC)
    {
      this->board->unlock();
      delay(this->appConfig->config.relayDelay);
      this->board->lock();
    } else {
      this->board->beepOk();
    }
    
    this->doorlock->saveEvent(
          uid,
          EventType::IN,
          Utils::now());
  }

  void testDoorOpened()
  {
    if (!this->unlocked && this->board->isLocked() && this->board->isDoorOpened())
    {
      this->board->beepNotOk();
      Serial.println(F("[LOG]: Door opened alert!"));

      time_t t = Utils::now();
      time_t diff = t - this->lastDoorOpenTime; // seconds
      if (diff > this->appConfig->config.doorOpenedAlertDelay && !this->hasDoorOpenEvent)
      {
          Serial.println(F("[LOG]: Door opened event registered!"));
          this->hasDoorOpenEvent = true;
          this->doorlock->saveEvent(
            this->lastUid,
            EventType::OPENED,
            Utils::now());
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

  void testCommandButton()
  {
    static time_t lastCommandButtonTime = 0;
    if (this->board->isCommandButtonPushed() && (Utils::now() - lastCommandButtonTime > 3))
    {
      lastCommandButtonTime = Utils::now();
      if (this->board->isCommandButtonPushed())
        if(this->unlocked || this->board->isLightOn())
          this->openDoor(UID_NULL);
        else 
          this->board->beepNotOk();
    }
  }

  void blink()
  {
    this->board->blinkActivityLed();
    if (this->unlocked)
    {
      this->board->blinkActivityLed();
    }
  }

  void learnUid(Uid uid)
  {
    if (this->doorlock->learnUid(uid))
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

  void checkAccess(Uid uid)
  {
    if (this->doorlock->checkAccess(uid))
    {
      switch(this->doorlock->getLastUserType()){
        // Keep door opened when a professor open it
        case UserType::PROFESSOR: this->toggleDoor(uid); break;
        case UserType::EMPLOYEE: 
          // Allow employ closes an opened door
          if (this->board->isLocked())
            this->openDoor(uid);
          else
            this->toggleDoor(uid);
          break;
        default:
            this->openDoor(uid);
        }
    }
    else
    {
      // If the door is unlocked, save IN event with uid
      if (this->unlocked)
        this->openDoor(uid);
      else
        this->board->beepNotOk();
    }
  }

  void readUid()
  {
    Uid uid = this->reader->readUid();
    if (uid == UID_NULL)
      return;

    this->lastUid = uid;
    this->lastUidTime = Utils::now();

    Serial.print("[LOG]: UID: ");
    Serial.println(uid);

    if (this->board->isProgramButtonPushed())
      this->learnUid(uid);
    else
      this->checkAccess(uid);
  }

  void run()
  {
    static unsigned long lastRead = 0;
    unsigned long now = millis();
    this->readUid();
    this->testCommandButton();
    if (now - lastRead > 900) // Read UID every second
    {
      lastRead = now;
      this->testDoorOpened();
      this->testShutdownNow();
      this->blink();
    }
  }
};

#endif