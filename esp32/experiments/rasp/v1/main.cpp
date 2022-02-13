#include <MFRC522.h>
#include <Time.h>
#include "SPIFFS.h"

#include "models.cpp"
#include "sqlite-db.cpp"
#include "config.cpp"

#define FORMAT_SPIFFS_IF_FAILED true

class AppDb : public SqliteDB
{
public:
  AppDb(const char *filename) : SqliteDB(filename) {}

  void startup()
  {
    int rc = this->initSession();
    if (rc != SQLITE_OK)
      return;

    this->exec("DROP TABLE IF EXISTS events;");
    this->exec("CREATE TABLE IF NOT EXISTS events (\n"
               "  id INTEGER NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  time datetime NOT NULL,\n"
               "  eventType varchar(4) NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");\n");
    this->exec("DROP TABLE IF EXISTS keyrings;");
    this->exec("CREATE TABLE IF NOT EXISTS keyrings (\n"
               "  id INTEGER NOT NULL,\n"
               "  uid varchar(16) NOT NULL,\n"
               "  userId INTEGER NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");
    this->exec("DROP TABLE IF EXISTS schedules;");
    this->exec("CREATE TABLE IF NOT EXISTS schedules (\n"
               "  id INTEGER NOT NULL,\n"
               "  dayOfWeek varchar(10) NOT NULL,\n"
               "  beginTime time NOT NULL,\n"
               "  endTime time NOT NULL,\n"
               "  userType varchar(10) NOT NULL,\n"
               "  lastUpdate datetime NOT NULL,\n"
               "  PRIMARY KEY (id)\n"
               ");");

    this->exec("INSERT INTO keyrings VALUES "
               "(1,'28:D0:9F:59:3E',1,'PROFESSOR','0000-00-00 00:00:00'),"
               "(2,'F6:B0:91:89:5E',1,'STUDENT','0000-00-00 00:00:00'),"
               "(3,'1A008D81DDCB',1,'PROFESSOR','0000-00-00 00:00:00');");
    this->exec("INSERT INTO schedules VALUES "
               "(1,'SUNDAY','00:00:00','23:00:00','STUDENT','0000-00-00 00:00:00');");

    this->closeSession();
  }

  void updateEvents()
  {
    // this->execWith("SELECT * from events;", );
  }
  void updateSchedules()
  {
    // this->execWith("");
  }
  void updateKeyrings()
  {
    //
  }
};

// Run forever, each 2 seconds
void tasks(AppDb *db)
{
  db->updateKeyrings();
  sleep(UPDATE_DELAY);
  db->updateSchedules();
  sleep(UPDATE_DELAY);
  db->updateEvents();
  sleep(UPDATE_DELAY);
}

AppDb db("/spiffs/database.db");

void setup()
{
  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);

  // Fake timer
  setTime(17, 20, 0, 2, 2, 2021);

  // Using RTC as provider of time
  // setSyncProvider(RTC.get);

  // Initialize file system handle
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("[ERROR]: Failed to mount file system");
  }

  // db.init();
  // db.startup();
}

void loop() {}
