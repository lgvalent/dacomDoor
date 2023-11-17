#ifndef APP_TASKS
#define APP_TASKS
#include "SPIFFS.h"
#include <Vector.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config.cpp"
#include "commons-db.cpp"

#define FORMAT_SPIFFS_IF_FAILED true

static String vector_to_string(Vector<String> &xs, int idx_start, int idx_end)
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
static String vector_to_string(Vector<String> &xs) { return vector_to_string(xs, 0, xs.size()); }

static boolean is_valid_json(JsonObject doc, Vector<String> keys)
{
  for (String key : keys)
    if (!doc.containsKey(key) || doc[key].isNull())
      return false;
  return true;
}

class AppTasks
{
protected:
  SqliteDB *db;
  time_t lastUpdate;
  int state = 0; // [0-3]: ["Keyrings", "Schedule", "Events"]

  int networkGuard()
  {
    if (!this->hasNetwork())
    {
      Serial.println(F("[WARN]: Network was not connected"));
      return 1;
    }
    return 0;
  }

  int sendUpdateEventsRequest(Vector<String> &result)
  {
    HTTPClient http;

    http.begin(URL_SERVER + "/doorlock/" + ROOM_NAME + "/events");
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(vector_to_string(result));

    if (code == 200)
      Serial.println("[LOG]: All events update");
    else
    {
      Serial.println("[ERROR]: Fail to update events");
      Serial.print("[ERROR]: StatusCode: ");
      Serial.println(code);
    }

    http.end();

    return code;
  }
  int sendUpdateScheduleRequest(String &lastUpdate)
  {
    HTTPClient http;

    http.begin(URL_SERVER + "/doorlock/" + ROOM_NAME + "/schedules?lastUpdate=\"" + lastUpdate + "\"");
    http.addHeader("Content-Type", "application/json");

    int code = http.GET();

    if (code == 204) // NO CONTENT
    {
    }
    else if (code != 200)
    {
      Serial.println("[ERROR]: Fail to update schedules");
      Serial.print("[ERROR]: StatusCode: ");
      Serial.println(code);
    }
    else
    {
      String strJson = http.getString();
      this->updateInternalSchedules(strJson);
    }

    http.end();
    return code;
  }
  int sendUpdateKeyringsRequest(String &lastUpdate)
  {
    HTTPClient http;

    http.begin(URL_SERVER + "/doorlock/" + ROOM_NAME + "/keyrings?lastUpdate=\"" + lastUpdate + "\"");
    http.addHeader("Content-Type", "application/json");

    int code = http.GET();

    if (code == 204) // NO CONTENT
    {
    }
    else if (code != 200)
    {
      Serial.println("[ERROR]: Fail to update schedules");
      Serial.print("[ERROR]: StatusCode: ");
      Serial.println(code);
    }
    else
    {
      String strJson = http.getString();
      this->updateInternalKeyrings(strJson);
    }
    http.end();
    return code;
  }

  void updateInternalKeyrings(String &strJson)
  {
    // TODO: Find how to check types of JSON result
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, strJson);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else if (doc.containsKey("removed") && doc.containsKey("updated"))
    {
      KeyringsModel keyringsModel;

      Vector<String> removedKeys;
      removedKeys.push_back("userId");

      for (JsonObject x : doc["removed"].as<JsonArray>())
      {
        if (is_valid_json(x, removedKeys))
        {
          long userId = x["userId"];
          keyringsModel.setUserId(userId);

          this->db->getById(keyringsModel);

          if (this->db->hasResult())
          {
            this->db->remove(keyringsModel);
            Serial.print("[LOG]: keyring removed: ");
            Serial.println(userId);
          }
          else
          {
            Serial.print("[LOG]: keyring not found to remove: ");
            Serial.println(userId);
          }
        }
        else
        {
          Serial.println("[WARN]: An invalid removed result was found!");
        }
      }

      Vector<String> updatedKeys;
      updatedKeys.push_back("uid");
      updatedKeys.push_back("userId");
      updatedKeys.push_back("userType");
      updatedKeys.push_back("lastUpdate");

      for (JsonObject x : doc["updated"].as<JsonArray>())
      {
        if (is_valid_json(x, updatedKeys))
        {
          long userId = x["userId"];
          const char *uid = x["uid"];
          const char *userType = x["userType"];     // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
          const char *lastUpdate = x["lastUpdate"]; // datatime
          keyringsModel.build(uid, userId, userType, String(lastUpdate));

          this->db->getById(keyringsModel);

          if (this->db->hasResult())
          {
            this->db->update(keyringsModel);
            Serial.print("[LOG]: keyring updated: ");
            Serial.println(userId);
          }
          else
          {
            this->db->add(keyringsModel);
            Serial.print("[LOG]: keyring added: ");
            Serial.println(userId);
          }
        }
        else
        {
          Serial.println("[WARN]: An invalid updated result was found!");
        }
      }
    }
    else
    {
      Serial.println("[WARN]: Request result json was different of specification!");
    }
  }
  void updateInternalSchedules(String &strJson)
  {
    // TODO: Find how to check types of JSON result
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, strJson);

    // Test if parsing succeeds.
    if (error)
    {
      Serial.print(F("[ERROR]: deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else if (doc.containsKey("removed") && doc.containsKey("updated"))
    {
      SchedulesModel schedulesModel;

      Vector<String> removedKeys;
      removedKeys.push_back("id");

      for (JsonObject x : doc["removed"].as<JsonArray>())
      {
        if (is_valid_json(x, removedKeys))
        {
          long id = x["id"];
          schedulesModel.setId(id);

          this->db->getById(schedulesModel);

          if (this->db->hasResult())
          {
            this->db->remove(schedulesModel);
            Serial.print("[LOG]: schedule removed: ");
            Serial.println(id);
          }
          else
          {
            Serial.print("[LOG]: schedule not found to remove: ");
            Serial.println(id);
          }
        }
        else
        {
          Serial.println("[WARN]: An invalid removed result was found!");
        }
      }

      Vector<String> updatedKeys;
      updatedKeys.push_back("id");
      updatedKeys.push_back("dayOfWeek");
      updatedKeys.push_back("beginTime");
      updatedKeys.push_back("endTime");
      updatedKeys.push_back("userType");
      updatedKeys.push_back("lastUpdate");

      for (JsonObject x : doc["updated"].as<JsonArray>())
      {
        if (is_valid_json(x, updatedKeys))
        {
          long id = x["id"];
          const char *dayOfWeek = x["dayOfWeek"];
          const char *beginTime = x["beginTime"];   // time
          const char *endTime = x["endTime"];       // time
          const char *userType = x["userType"];     // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
          const char *lastUpdate = x["lastUpdate"]; // datatime
          schedulesModel.build(id, dayOfWeek, beginTime, String(endTime), userType, String(lastUpdate));

          this->db->getById(schedulesModel);

          if (this->db->hasResult())
          {
            this->db->exec(schedulesModel.update().c_str());
            Serial.print("[LOG]: schedule updated: ");
            Serial.println(id);
          }
          else
          {
            this->db->update(schedulesModel);
            Serial.print("[LOG]: schedule added: ");
            Serial.println(id);
          }
        }
        else
        {
          Serial.println("[WARN]: An invalid updated result was found!");
        }
      }
    }
    else
    {
      Serial.println("[WARN]: Request result json was different of specification!");
    }
  }

  void updateEvents()
  {
    if (this->networkGuard())
      return;

    this->db->initSession();
    Vector<String> result;

    this->db->execWithContext(
        "SELECT id, uid, time, eventType FROM events;", &result,
        [](void *ctx, int n, char **data, char **cols) {
          db_query_returned = true;
          Vector<String> *rs = (Vector<String> *)ctx;
          String json = EventsModel::JSON_TEMPLATE();
          json.replace("$1", String(data[0])); // id
          json.replace("$2", String(data[1])); // uid
          json.replace("$3", String(data[2])); // time
          json.replace("$4", String(data[3])); // eventType
          rs->push_back(json);
          return 0;
        });

    if (this->db->hasResult())
    {
      int code = this->sendUpdateEventsRequest(result);

      if (code == 200)
        this->db->exec("DELETE FROM events;");
    }

    this->db->closeSession();
  }
  void updateSchedules()
  {
    if (this->networkGuard())
      return;

    this->db->initSession();
    this->db->exec1("SELECT lastUpdate\nFROM schedules\nORDER BY lastUpdate DESC\nLIMIT 1;");

    String lastUpdate = ROOM_LAST_UPDATE_FAKE;

    if (this->db->hasResult())
      lastUpdate = first_row.getCol("lastUpdate");

    this->sendUpdateScheduleRequest(lastUpdate);

    this->db->closeSession();
  }
  void updateKeyrings()
  {
    if (this->networkGuard())
      return;
    this->db->initSession();

    this->db->exec1("SELECT lastUpdate\nFROM keyrings\nORDER BY lastUpdate DESC\nLIMIT 1;");

    String lastUpdate = ROOM_LAST_UPDATE_FAKE;

    if (this->db->hasResult())
      lastUpdate = first_row.getCol("lastUpdate");

    this->sendUpdateKeyringsRequest(lastUpdate);

    this->db->closeSession();
  }

  void startupDatabase()
  {
    int rc = this->db->initSession();
    if (rc != SQLITE_OK)
      return;

    this->db->exec("DROP TABLE IF EXISTS events;");
    this->db->exec("CREATE TABLE IF NOT EXISTS events (\n"
                   "  uid varchar(16) NOT NULL,\n"
                   "  time datetime NOT NULL,\n"
                   "  eventType varchar(4) NOT NULL,\n"
                   ");\n");
    this->db->exec("DROP TABLE IF EXISTS keyrings;");
    this->db->exec("CREATE TABLE IF NOT EXISTS keyrings (\n"
                   "  userId INTEGER NOT NULL,\n"
                   "  uid varchar(16) NOT NULL,\n"
                   "  userType varchar(10) NOT NULL,\n"
                   "  lastUpdate datetime NOT NULL,\n"
                   ");");
    this->db->exec("DROP TABLE IF EXISTS schedules;");
    this->db->exec("CREATE TABLE IF NOT EXISTS schedules (\n"
                   "  id INTEGER NOT NULL,\n"
                   "  dayOfWeek varchar(10) NOT NULL,\n"
                   "  beginTime time NOT NULL,\n"
                   "  endTime time NOT NULL,\n"
                   "  userType varchar(10) NOT NULL,\n"
                   "  lastUpdate datetime NOT NULL,\n"
                   ");");

    this->db->exec("INSERT INTO keyrings VALUES "
                   "('28:D0:9F:59:3E',1,'PROFESSOR','0000-00-00 00:00:00'),"
                   "('F6:B0:91:89:5E',1,'STUDENT','0000-00-00 00:00:00'),"
                   "('1A008D81DDCB',1,'PROFESSOR','0000-00-00 00:00:00');");
    this->db->exec("INSERT INTO schedules VALUES "
                   "('SUNDAY','00:00:00','23:00:00','STUDENT','0000-00-00 00:00:00');");

    this->db->closeSession();
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

public:
  boolean hasNetwork() { return WiFi.isConnected(); }

  AppTasks(SqliteDB *db) : db(db) { this->lastUpdate = time(NULL); }

  void startup()
  {
    this->startupDatabase();
    this->startupNetwork();
  }

  void run()
  {
    // time_t represents the number of seconds from 1970 until now.
    time_t t = time(NULL);
    time_t diff = t - this->lastUpdate;
    if (diff > UPDATE_DELAY)
    {
      int lastState = this->state;
      Serial.print("[LOG]: Ellapse time: ");
      Serial.println(diff);

      this->lastUpdate = t;
      switch (this->state)
      {
      case 0:
        this->updateKeyrings();
        this->state = 1;
        break;
      case 1:
        this->updateSchedules();
        this->state = 2;
        break;
      case 2:
        this->updateEvents();
        this->state = 0;
        break;
      default:
        this->state = 0;
        break;
      }
      Serial.print("[LOG] state: ");
      Serial.print(lastState);
      Serial.print(" -> ");
      Serial.println(this->state);

      Serial.println();
    }
  }
};

#endif