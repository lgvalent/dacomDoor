#include <MFRC522.h>
#include <Time.h>
#include "SPIFFS.h"
#include <Vector.h>
#include <WiFi.h>

#include "models.cpp"
#include "sqlite-db.cpp"
#include "config.cpp"

#define FORMAT_SPIFFS_IF_FAILED true

String vector_to_string(Vector<String> xs, int idx_start, int idx_end)
{
  String res = "[";

  int limit = idx_end - 1;
  for (int i = idx_start; i < limit; i++)
    res += xs.at(i) + ",";

  int length = idx_end - idx_start;
  if (length > 0)
    res += xs.at(limit);

  return res + "]";
}
String vector_to_string(Vector<String> xs) { return vector_to_string(xs, 0, xs.size()); }

class AppDb : public SqliteDB
{
private:
public:
  boolean hasNetwork() { return WiFi.isConnected(); }

  AppDb(const char *filename) : SqliteDB(filename) {}

  void startupDatabase()
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

  void startupNetwork()
  {
    // WIFI_STA: Station Mode (Client)
    // WIFI_AP: Access Point (Router)
    // WIFI_AP_STA: ! (Both)
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int count = 0;
    int wifi_connection_max_tries = 10;
    // Waiting for wifi connection
    while (count < wifi_connection_max_tries && WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Connecting to WiFi..");
      count++;
    }

    if (WiFi.status() != WL_CONNECTED)
      Serial.println("Fail to connect to WiFi!");
    else
      Serial.println("WiFi connected!");
  }

  void updateEvents()
  {
    this->initSession();
    Vector<String> result;

    this->execWithContext(
        "SELECT id, uid, time, eventType FROM events;", &result,
        [](void *ctx, int n, char **data, char **cols) {
          Vector<String> *rs = (Vector<String> *)ctx;
          String json = EventsModel::JSON_TEMPLATE();
          json.replace("$1", String(data[0])); // id
          json.replace("$2", String(data[1])); // uid
          json.replace("$3", String(data[2])); // time
          json.replace("$4", String(data[3])); // eventType
          rs->push_back(json);
          return 0;
        });

    Serial.println(vector_to_string(result));

    this->closeSession();
  }
  void updateSchedules()
  {
    this->initSession();
    // this->execWith("");
    this->closeSession();
  }
  void updateKeyrings()
  {
    this->initSession();
    //
    this->closeSession();
  }
};

// Run forever, each 2 seconds
void tasks(AppDb *app)
{
  app->updateKeyrings();
  sleep(UPDATE_DELAY);
  app->updateSchedules();
  sleep(UPDATE_DELAY);
  app->updateEvents();
  sleep(UPDATE_DELAY);
}

AppDb app("/spiffs/database.db");

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

  app.init();
  app.startupDatabase();
  app.startupNetwork();

  app.updateEvents();
}

void loop() {}
