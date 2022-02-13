#ifndef APP_BOARD
#define APP_BOARD
#include <SPI.h>

#include "commons.cpp"
#include "utils.cpp"
#include "board-models.cpp"
#include "doorlock.cpp"

class AppBoard
{
private:
  int blinkActivityLedCount = 0;
  const uint32_t BLINK_DELAY = 1000;

  BoardModel *board;
  Doorlock *doorlock;

  String *lastUid = NULL;
  time_t lastUidTime = 0;
  bool hasDoorOpenEvent;

  MFRC522 uidReader;

protected:
  void startupUidReader()
  {
    // Initialize SPI bus
    SPI.begin();

    // Init rfid reader
    this->uidReader.PCD_Init();
  }

public:
  time_t lastDoorOpenTime = 0;

  AppBoard(BoardModel *board, Doorlock *doorlock) : board(board),
                                                    doorlock(doorlock) {}

  void startup()
  {
    this->startupUidReader();
  }

  void toggleDoor(String uid)
  {
    this->board->toggleLocked();
    bool isLocked = this->board->isLocked();

    if (RELAY_DELAY > 1)
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

    if (RELAY_DELAY > 1 || isLocked)
    {
      String eventType = isLocked ? EventTypesEnum::OUT : EventTypesEnum::IN;
      this->doorlock->saveEvent(
          uid,
          eventType,
          Utils::currentDatetime());
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

  void openDoor(String uid)
  {
    uint32_t t = RELAY_DELAY * 1000.0;
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
    else if (this->lastUid != NULL)
    {
      time_t t = now();
      time_t diff = t - this->lastDoorOpenTime; // seconds
      if (diff > DOOR_OPENED_ALERT_DELAY)
      {
        this->board->beepOk();
        this->board->blinkActivityLed();
        bool notHasDoorOpenEvent = !this->hasDoorOpenEvent;
        if (notHasDoorOpenEvent)
        {
          this->hasDoorOpenEvent = true;
          this->doorlock->saveEvent(
            *this->lastUid,
            EventTypesEnum::DOOR_OPENED,
            Utils::currentDatetime()
          );
        }
      }
    }
  }

  void shutdownNow()
  {
    // TODO
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

  void testProgramButtonPush()
  {
    if (this->board->isProgramButtonPushed())
    {
      this->board->beepNotOk();
      this->board->blinkActivityLed();
      this->board->blinkActivityLed();
      this->board->blinkActivityLed();
      Serial.println("[LOG]: Bring RFID card closer to learn for local access");
    }
  }

  void blink()
  {
    this->blinkActivityLedCount++;
    this->board->blinkActivityLed();
    if (!this->board->isLocked() && this->blinkActivityLedCount > BLINK_DELAY)
    {
      this->board->blinkActivityLed();
      this->blinkActivityLedCount = 0;
    }
  }

  void tryLearnUid(String uid)
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

  void tryCheckAccess(String uid)
  {
    bool hasAccess = this->doorlock->checkAccess(uid);

    if (hasAccess)
    {
      String *userType = this->doorlock->getLastUserType();
      if (userType != NULL)
      {
        if (userType->equals(UserTypeEnum::PROFESSOR))
        {
          // Keep door opened when a professor open it
          this->toggleDoor(uid);
        }
        else if (userType->equals(UserTypeEnum::EMPLOYEE))
        {
          // Allow employ closes an opened door
          if (this->board->isLocked())
            this->openDoor(uid);
          else
            this->toggleDoor(uid);
        }
      }
    }
    else
    {
      this->board->beepNotOk();
    }
  }

  void readUid()
  {
    // Reset the loop if no new card present on the sensor/reader.
    // This saves the entire process when idle.
    if (!this->uidReader.PICC_IsNewCardPresent())
      return;

    // Verify if the UID has been read
    if (!this->uidReader.PICC_ReadCardSerial())
      return;

    String uid = Utils::uidBytesToString(
        this->uidReader.uid.uidByte,
        this->uidReader.uid.size);

    this->lastUid = &uid;
    this->lastUidTime = now();

    Serial.print("[LOG]: UID: ");
    Serial.println(uid);

    if (this->board->isProgramButtonPushed())
      this->tryLearnUid(uid);
    else
      this->tryCheckAccess(uid);

    // Halt PICC
    this->uidReader.PICC_HaltA();

    // Stop encryption on PCD
    this->uidReader.PCD_StopCrypto1();
  }

  void run()
  {
    this->testDoorOpened();
    this->testShutdownNow();
    this->testProgramButtonPush();
    this->blink();
    this->readUid();
  }
};

#endif