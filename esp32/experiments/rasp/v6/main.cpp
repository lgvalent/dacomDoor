#include <MFRC522.h>
#include <Time.h>

#include "doorlock.cpp"

DebugApp app("/spiffs/database.db");
Doorlock doorlock(&app);

void setup()
{
  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
}
