#include "commons.cpp"
#include "doorlock.cpp"
#include "board-models.cpp"

DebugAppTasks appTasks("/spiffs/database.db");
Doorlock doorlock(&appTasks);

class AppBoard
{
private:
  BoardModel *board;
  void *lastUid;
  void *lastUidTime;
  void *lastKeyring;
  time_t lastDoorOpenTime;
  void *lastDoorOpenEvent;

public:
  void setBoard(BoardModel *board) { this->board = board; }

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

  void readRfid()
  {
    // TODO
  }

  void run()
  {
    this->testShutdownNow();
    this->testProgramButtonPush();
    this->blink();
    this->readRfid();
  }
};

AppBoard boardModel;

void setup()
{
  BoardModel *board = getBoardModel(BOARD_VERSION);
  boardModel.setBoard(board);

  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
}
