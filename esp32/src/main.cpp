#include <Arduino.h>
#include "app-tasks.cpp"
#include "app-board.cpp"
#include "doorlock.cpp"
#include "config.cpp"
#include "reader.cpp"

Doorlock *doorlock;
BoardModel *boardModel;
AppTasks *appTasks;
AppBoard *appBoard;
AppConfig *appConfig;
Reader *reader;

// Task wrappers for lambdas
struct TaskWrapper {
  std::function<void()> func;
  static void run(void *arg) {
    TaskWrapper *task = static_cast<TaskWrapper *>(arg);
    task->func();
    vTaskDelete(NULL); // Just in case the task exits
  }
};

/** Callback functions precisam extremamente breve para não corromper a execução no ESP32 */
void doorSensorCallback()
{
  appBoard->lastDoorOpenTime = Utils::now();
  appBoard->hasDoorOpenEvent = false;
}

/** Callback functions precisam extremamente breve para não corromper a execução no ESP32 */
void commandButtonCallback()
{
  // if (boardModel->isLightOn() || appBoard->unlocked)
  //    ; //appBoard->openDoor(UID_NULL);
  //   else
  //     boardModel->beepNotOk();
}

void setup()
{
  Serial.begin(115200);
  Serial.printf("[MAIN] Booting v%s...\n", AppConfig::BUILD_INFO.c_str());

  appConfig = new AppConfig();
  appConfig->startup();

  doorlock = new Doorlock();
  boardModel = BoardModel::getBoardModel(appConfig->config.boardVersion);

  reader = new Reader(boardModel);
  reader->startup();

  appBoard = new AppBoard(appConfig, boardModel, doorlock, reader);
  appTasks = new AppTasks(appConfig);
  
  boardModel->serDoorSensorCallback(&doorSensorCallback);
  boardModel->setCommandButtonCallback(&commandButtonCallback);

  appBoard->startup();
  appTasks->startup();

  Serial.println("[MAIN] Creating multitask environment...");
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
      8192, /** Increasing STACK to use JSON and SQLite3  */
      taskRunner,
      1,
      nullptr,
      1  // Core 1
    );
  esp_sleep_enable_uart_wakeup(reader->getUartNumber()); // Enable UART wakeup for board version 6
  esp_sleep_enable_timer_wakeup(0.5 * 1000000); // Convert seconds to microseconds
  boardModel->beepNotOk(); 
  boardModel->beepOk(); 
  Serial.println("[MAIN] Setup finished...");
 
  /** For tests only */
  // appConfig->config.updateDelay = 0; 
  // esp_light_sleep_start(); // Start light sleep to save power until the first event
  // Serial.println("Entering light sleep...");
}

void loop()
{
    // Nothing to do here. Everything runs in tasks.
}
