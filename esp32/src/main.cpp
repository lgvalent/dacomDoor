#include <Arduino.h>
#include "app-tasks.cpp"
#include "app-board.cpp"
#include "doorlock.cpp"
#include "config.cpp"

time_t lastCommandButtonTime = 0;
time_t lastDoorSensorTime = 0;
bool locked = true;

Doorlock *doorlock;
BoardModel *boardModel;
AppTasks *appTasks;
AppBoard *appBoard;

// Task wrappers for lambdas
struct TaskWrapper {
  std::function<void()> func;
  static void run(void *arg) {
    TaskWrapper *task = static_cast<TaskWrapper *>(arg);
    task->func();
    vTaskDelete(NULL); // Just in case the task exits
  }
};

// Door sensor ISR callback
void doorSensorCallback()
{
  time_t t = Utils::now();
  if (t - lastDoorSensorTime > 1) {
    lastDoorSensorTime = t;
    appBoard->lastDoorOpenTime = t;
  }
}

// Command button ISR callback
void commandButtonCallback()
{
  time_t t = Utils::now();
  if (t - lastCommandButtonTime > 1) {
    if (boardModel->isLightOn() || boardModel->isLocked())
      appBoard->openDoor(UID_NULL);
    else
      boardModel->beepOk();
    lastCommandButtonTime = t;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000); // Give time for serial monitor

  startupConfig();

  doorlock = new Doorlock();
  boardModel = getBoardModel(config.boardVersion);
  appBoard = new AppBoard(boardModel, doorlock);
  appTasks = new AppTasks();

  boardModel->serDoorSensorCallback(&doorSensorCallback);
  boardModel->setCommandButtonCallback(&commandButtonCallback);

  // Create task for appBoard->run()
  static TaskWrapper *boardTask = new TaskWrapper{
    .func = []() {
      while (true) {
        appBoard->run();
        vTaskDelay(10 / portTICK_PERIOD_MS);  // Adjust as needed
      }
    }
  };

  xTaskCreatePinnedToCore(
    TaskWrapper::run,
    "AppBoardTask",
    4096,
    boardTask,
    1,
    nullptr,
    0  // Core 0
  );

  // Create task for appTasks->run()
  static TaskWrapper *taskRunner = new TaskWrapper{
    .func = []() {
      while (true) {
        appTasks->run();
        vTaskDelay(100 / portTICK_PERIOD_MS); // Adjust as needed
      }
    }
  };

  xTaskCreatePinnedToCore(
    TaskWrapper::run,
    "AppTasksTask",
    4096,
    taskRunner,
    1,
    nullptr,
    1  // Core 1
  );
}

void loop()
{
  // Nothing to do here. Everything runs in tasks.
}
