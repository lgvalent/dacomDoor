#include "commons.cpp"
#include "app-tasks.cpp"
#include "app-board.cpp"

time_t lastCommandButtonTime = 0;
time_t lastDoorSensorTime = 0;
bool locked = true;

SqliteDB db("/spiffs/database.db");
AppTasks appTasks(&db);
Doorlock doorlock(&db);
BoardModel *boardModel;
AppBoard *appBoard;

void doorSensorCallback()
{
  time_t t = now();
  if (t - lastDoorSensorTime > 1)
  {
    lastDoorSensorTime = t;
    appBoard->lastDoorOpenTime = t;
  }
}

void commandButtonCallback()
{
  time_t t = now();
  if (t - lastCommandButtonTime > 1)
  {
    if (boardModel->isLightOn() || boardModel->isLocked())
      appBoard->openDoor("");
    else
      boardModel->beepOk();

    lastCommandButtonTime = t;
  }
}

void setup()
{
  boardModel = getBoardModel(BOARD_VERSION);
  appBoard = new AppBoard(boardModel, &doorlock);
  
  boardModel->serDoorSensorCallback(&doorSensorCallback);
  boardModel->setCommandButtonCallback(&commandButtonCallback);

  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);

}

void loop()
{
  appBoard->run();
  appTasks->run();
}
