#include "SPIFFS.h"
#include <FS.h>
#include <MFRC522.h>
#include <sqlite3.h>

sqlite3 *db;

const char STARTUP_SQL_FILEPATH[] = "/startup.sql";

void setup()
{
  // Initialize file system handle
  if (!SPIFFS.begin(true))
  {
    Serial.println("[ERROR]: Failed to mount file system");
    return;
  }

  // Wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);

  File file = SPIFFS.open(STARTUP_SQL_FILEPATH, FILE_READ);

  while (file.available()) {
    String cmd = file.readStringUntil(';') + ";";
    Serial.println(cmd);
  }
}

void loop()
{
}
