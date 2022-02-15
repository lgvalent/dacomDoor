#include <FS.h>
#include "SPIFFS.h"
#include <MFRC522.h>

const char FILENAME_TO_WRITE[] = "/startup.sql";
File file;
int running = 1;

void setup()
{
  // Initialize file system handle
  if (!SPIFFS.begin(true))
  {
    Serial.println("[ERROR]: Failed to mount file system");
  }
  else
  {
    // Wait a time to Arduino Serial Monitor opens
    Serial.begin(115200);
    delay(1000);

    Serial.println("Start write:");
    file = SPIFFS.open(FILENAME_TO_WRITE, "w");
  }
}

void loop()
{
  while (running && Serial.available())
  {
    char ch = Serial.read();
    if (ch == 168)
    {
      Serial.println("File closed!");
      file.close();
      running = 0;
    }
    else
    {
      file.print(String(ch));
    }
  }
}
