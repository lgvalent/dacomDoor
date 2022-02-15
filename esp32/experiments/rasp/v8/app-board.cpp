#ifndef APP_BOARD
#define APP_BOARD
#include <SPI.h>
#include "utils.cpp"
#include "board-models.cpp"
#include "doorlock.cpp"

class AppBoard
{
private:
  IBoardModel *board;
  Doorlock *doorlock;

  String *lastUid = NULL;
  time_t lastUidTime = 0;
  void *lastKeyring;
  time_t lastDoorOpenTime = 0;
  void *lastDoorOpenEvent;

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
  AppBoard(IBoardModel *board, Doorlock *doorlock) : board(board),
                                                    doorlock(doorlock) {}

  void startup()
  {
    this->startupUidReader();
  }

  void toggleDoor(String uid)
  {
    // TODO
  }

  void openDoor(String uid)
  {
    // TODO
  }

  void shutdownNow()
  {
    // TODO
  }

  void testShutdownNow()
  {
    // if (this->isProgramButtonPushed() && this->isCommandButtonPushed())
    if (false)
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
    this->board->blinkActivityLed();
    if (!this->board->isLocked())
      this->board->blinkActivityLed();
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
    this->testShutdownNow();
    this->testProgramButtonPush();
    this->blink();
    this->readUid();
  }
};

#endif