#include "app-tasks.cpp"
#include "app-board.cpp"

DebugDbNoopLog db("/spiffs/database.db");
AppTasks appTasks(&db);

void setup()
{
  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
}
