#include "SPIFFS.h"
#include <FS.h>
#include <MFRC522.h>

const char FILENAME_TO_WRITE[] = "/startup.sql";

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

    Serial.println("Start reading file");
    File file = SPIFFS.open(FILENAME_TO_WRITE, FILE_READ);
    char ch;
    while (file.available() && (ch = file.read()) > 0)
    {
      Serial.print(ch);
    }
    Serial.println("\nEnd reading file");
  }
}

void loop()
{
}
