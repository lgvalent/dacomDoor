// #include <FS.h>
#include <MFRC522.h>
// #include <SPI.h>
// #include <Time.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// Constants
const int UPDATE_DELAY = 60;
const double RELAY_DELAY = 0.1;
const int DOOR_OPENED_ALERT_DELAY = 30;
const char SQLITE_FILEPATH[] = "/spiffs/database.db";

void initialize_database()
{
}

void setup()
{
  // Wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);
}

int count = 0;
void loop()
{
  Serial.println(F("LOOPING"));
  Serial.println(count++);
  delay(1000);
}
