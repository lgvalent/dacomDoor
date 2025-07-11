#ifndef APP_TASKS
#define APP_TASKS
#include "SPIFFS.h"
#include <Vector.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "config.cpp"
#include "models.cpp"
#include "daosqlite3.cpp"

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
  KeyringDao keyringDao;
  EventDao eventDao;
  ScheduleDao scheduleDao;

  time_t lastUpdate;
  int state = 0; // [0-3]: ["Keyring", "Schedule", "Events"]

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

    http.begin(config.serverURL + "/doorlock/" + config.roomName + "/events");
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
  int sendUpdateScheduleRequest(time_t lastUpdate)
  {
    HTTPClient http;

    http.begin(config.serverURL + "/doorlock/" + config.roomName + "/schedules?lastUpdate=\"" + Utils::datetimeToString(lastUpdate) + "\"");
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
  int sendUpdateKeyringsRequest(time_t lastUpdate)
  {
    HTTPClient http;

    http.begin(config.serverURL + "/doorlock/" + config.roomName + "/keyring?lastUpdate=\"" + Utils::datetimeToString(lastUpdate) + "\"");
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
      
      Vector<String> removedKeys;
      removedKeys.push_back("userId");
      
      for (JsonObject x : doc["removed"].as<JsonArray>())
      {
        if (is_valid_json(x, removedKeys))
        {
          Uid userId = x["userId"];
          Keyring keyring = keyringDao.findByUid(userId);

          if (keyring.isValid())
          {
            keyringDao.remove(userId);
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
          Serial.println("[WARN]: An invalid removed entry was found!");
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
          Uid userId = x["userId"];
          Uid uid = x["uid"];
          const char *userTypeName = x["userType"];     // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
          const char *lastUpdate = x["lastUpdate"]; // datatime
          
          UserType userType = Utils::findEnumByValue(userTypeNames, userTypeName);

          Keyring keyring = keyringDao.findByUserId(userId);
          
          if (keyring.isValid())
          {
            keyring.build(uid, userId, userType, Utils::stringToDatetime(lastUpdate));
            keyringDao.update(keyring);
            Serial.print("[LOG]: keyring updated: ");
            Serial.println(userId);
          }
          else
          {
            keyring.build(uid, userId, userType, Utils::stringToDatetime(lastUpdate));
            keyringDao.save(keyring);
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
      
      Vector<String> removedKeys;
      removedKeys.push_back("id");
      
      for (JsonObject x : doc["removed"].as<JsonArray>())
      {
        if (is_valid_json(x, removedKeys))
        {
          Uid id = x["id"];
          Schedule schedule = scheduleDao.findById(id);

          if (schedule.isValid())
          {
            scheduleDao.remove(id);
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
          Serial.println("[WARN]: An invalid removed entry was found!");
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
          Uid id = x["id"];
          const char *dayOfWeek = x["dayOfWeek"];
          const char *beginTime = x["beginTime"];   // time
          const char *endTime = x["endTime"];       // time
          const char *userType = x["userType"];     // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
          const char *lastUpdate = x["lastUpdate"]; // datatime
          
          Schedule schedule = scheduleDao.findById(id);
          
          if (schedule.isValid())
          {
            schedule.build(id, Utils::findEnumByValue(dayOfWeekNames, String(dayOfWeek)), Utils::stringToDatetime(beginTime), Utils::stringToDatetime(endTime), Utils::findEnumByValue(userTypeNames, String(userType)), Utils::stringToDatetime(lastUpdate));
            scheduleDao.update(schedule);
            Serial.print("[LOG]: schedule updated: ");
            Serial.println(id);
          }
          else
          {
            schedule.build(id, Utils::findEnumByValue(dayOfWeekNames, String(dayOfWeek)), Utils::stringToDatetime(beginTime), Utils::stringToDatetime(endTime), Utils::findEnumByValue(userTypeNames, String(userType)), Utils::stringToDatetime(lastUpdate));
            scheduleDao.save(schedule);
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

    Vector<String> rs;

    eventDao.process(
        [&rs](const Event &event) {
          rs.push_back(event.toJSON_());
          return true;
        });

    if (rs.size())
    {
      int code = this->sendUpdateEventsRequest(rs);

      if (code == 200)
        eventDao.removeAll();
    }
  }
  void updateSchedules()
  {
    if (this->networkGuard())
      return;

    time_t lastUpdate = scheduleDao.getLastUpdate();

    this->sendUpdateScheduleRequest(lastUpdate);
  }
  void updateKeyrings()
  {
    if (this->networkGuard())
      return;

    time_t lastUpdate = keyringDao.getLastUpdate();

    this->sendUpdateKeyringsRequest(lastUpdate);
  }

  void startupNetwork()
  {
    // WIFI_STA: Station Mode (Client)
    // WIFI_AP: Access Point (Router)
    // WIFI_AP_STA: ! (Both)
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.wifiSSID.c_str(), config.wifiPassword.c_str());

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

  AppTasks(){ this->lastUpdate = time(NULL); }

  void startup()
  {
    this->startupNetwork();
  }

  void run()
  {
    // time_t represents the number of seconds from 1970 until now.
    time_t t = time(NULL);
    time_t diff = t - this->lastUpdate;
    if (diff > config.updateDelay)
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