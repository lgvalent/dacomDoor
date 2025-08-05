#ifndef APP_TASKS
#define APP_TASKS
#include "SPIFFS.h"
#include <WiFi.h>
#include <HTTPClient.h>

#include "config.cpp"
#include "models.cpp"
#include "daosqlite3.cpp"

#define FORMAT_SPIFFS_IF_FAILED true

static String vector_to_string(std::vector<String> &xs, int idx_start, int idx_end)
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
static String vector_to_string(std::vector<String> &xs) { return vector_to_string(xs, 0, xs.size()); }

// static boolean is_valid_json(JsonObject doc, std::vector<String> keys)
// {
//   for (String key : keys)
//     if (!doc.containsKey(key) || doc[key].isNull())
//       return false;
//   return true;
// }

#include <JsonStreamingParser.h>
#include <JsonListener.h>

class KeyringJsonHandler : public JsonListener {
private:
  enum Section { NONE, REMOVED, UPDATED } section = NONE;
  String currentKey;
  String uid, userId, userType, lastUpdate;
  bool insideObject = false;
  DaoManager *daoManager;

public:
  KeyringJsonHandler(DaoManager *daoManager): daoManager(daoManager) {}

  void key(String key) override {
    currentKey = key;
  
    // Se ainda não entramos em REMOVED ou UPDATED, estamos na raiz
    if (key == "removed" || key == "updated") {
      section = (key == "removed") ? REMOVED : UPDATED;
    }  
  }

  void value(String value) override {
    if (!insideObject) return;

    if (section == REMOVED && currentKey == "userId") {
      Uid uid = value.toInt();
      Keyring k = this->daoManager->keyringDao.findByUid(uid);
      if (k.isValid()) {
        this->daoManager->keyringDao.remove(uid);
        Serial.print("[LOG]: Removed keyring: ");
      }else{
        Serial.print("[LOG]: Removed keyring not found: ");
      }
      Serial.println(uid);
    }

    if (section == UPDATED) {
      if (currentKey == "uid") uid = value;
      else if (currentKey == "userId") userId = value;
      else if (currentKey == "userType") userType = value;
      else if (currentKey == "lastUpdate") lastUpdate = value;
    }
  }

  void startArray() override {}
  void endArray() override {}

  void startObject() override {
    insideObject = true;
    uid = userId = userType = lastUpdate = "";
  }

  void endObject() override {
    if (section == UPDATED && userId != "") {
      Keyring k = this->daoManager->keyringDao.findByUserId(userId.toInt());
      Uid uidInt = std::stoi(uid.c_str(), nullptr, 16); // Assuming uid is in hexadecimal format
      UserType ut = Utils::findEnumByValue(userTypeNames, userType);
      k.build(uidInt, userId.toInt(), ut, Utils::stringToDatetime(lastUpdate));
      this->daoManager->keyringDao.save(k);
      Serial.print("[LOG]: Updated keyring: ");
      Serial.println(userId);
    }

    insideObject = false;
  }

  void startDocument() override {}
  void endDocument() override {}

  void whitespace(char c) override {}

};


class AppTasks
{
protected:
  DaoManager *daoManager;
  AppConfig *appConfig;

  bool timeUpdated = false;
  time_t lastUpdateTry = 0;
 
  bool networkGuard()
  {
    if (!this->hasNetwork())
    {
      Serial.println(F("[WARN]: Network was not connected"));
      return false;
    }
    return true;
  }

  void updateDateTime()
  {
    Serial.print("Aguardando sincronização NTP...");
    configTime(this->appConfig->config.gmtZone*3600, 0, "pool.ntp.org", "time.nist.gov");
    struct tm timeinfo;
    int retry = 0;
    const int retry_count = 10;
    while (!getLocalTime(&timeinfo) && retry < retry_count) {
      Serial.print(".");
      delay(1000);
      retry++;
    }
    if (retry < retry_count) {
      Serial.printf("\nNTP sincronizado: %s\n", asctime(&timeinfo));
      this->timeUpdated = true;
    } else {
      Serial.print("\n[WARN]: Falha ao sincronizar NTP\n");
    }
  }

  bool sendUpdateKeyringsRequest()
{
  String url = this->appConfig->config.serverURL + "/doorlock/" + this->appConfig->config.roomName + "/keyrings?lastUpdate=" + Utils::datetimeToString(this->appConfig->config.lastUpdate);
  url.replace(" ", "%20");

  Serial.print("[LOG]: Sending keyrings update request to: ");
  Serial.println(url);

  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int code = http.GET();

  if (code != 200 && code != 204) {
    Serial.print("[ERROR]: StatusCode: ");
    Serial.println(code);
    http.end();
    return false;
  }

  if (code == 204) {
    http.end();
    return true;
  }

  WiFiClient& stream = http.getStream();

  KeyringJsonHandler handler(daoManager);
  JsonStreamingParser parser;
  parser.setListener(&handler);

  while (stream.connected() && stream.available()) {
    char c = stream.read();
    parser.parse(c);
  }

  http.end();
  return true;
}

  bool sendUpdateEventsRequest(std::vector<String> &result)
  {
    String url = this->appConfig->config.serverURL + "/doorlock/" + this->appConfig->config.roomName + "/events";
    Serial.print("[LOG]: Sending events update request to: "); Serial.println(url);
  
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(vector_to_string(result));

    if (code == 200)
      Serial.println("[LOG]: All events update");
    else
    {
      Serial.printf("[ERROR]: Fail to update events. Code %d\n", code);
    }

    http.end();

    return code==200 || code==204;
  }
  bool sendUpdateScheduleRequest()
  {
    String url = this->appConfig->config.serverURL + "/doorlock/" + this->appConfig->config.roomName + "/schedules?lastUpdate=" + Utils::datetimeToString(this->appConfig->config.lastUpdate);
    url.replace(" ", "%20"); // URL encode spaces
    Serial.print("[LOG]: Sending schedules update request to: "); Serial.println(url);

    HTTPClient http;
    http.begin(url);
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
    return code==200 || code==204;
  }

  void updateInternalSchedules(String &strJson)
  {
    // // TODO: Find how to check types of JSON result
    // DynamicJsonDocument doc(2048);
    // DeserializationError error = deserializeJson(doc, strJson);

    // // Test if parsing succeeds.
    // if (error)
    // {
    //   Serial.print(F("[ERROR]: deserializeJson() failed: "));
    //   Serial.println(error.f_str());
    //   return;
    // }
    // else if (doc.containsKey("removed") && doc.containsKey("updated"))
    // {
      
    //   std::vector<String> removedKeys;
    //   removedKeys.push_back("id");
      
    //   for (JsonObject x : doc["removed"].as<JsonArray>())
    //   {
    //     if (is_valid_json(x, removedKeys))
    //     {
    //       Uid id = x["id"];
    //       Schedule schedule = daoManager->scheduleDao.findById(id);

    //       if (schedule.isValid())
    //       {
    //         daoManager->scheduleDao.remove(id);
    //         Serial.print("[LOG]: schedule removed: ");
    //         Serial.println(id);
    //       }
    //       else
    //       {
    //         Serial.print("[LOG]: schedule not found to remove: ");
    //         Serial.println(id);
    //       }
    //     }
    //     else
    //     {
    //       Serial.println("[WARN]: An invalid removed entry was found!");
    //     }
    //   }

    //   std::vector<String> updatedKeys;
    //   updatedKeys.push_back("id");
    //   updatedKeys.push_back("dayOfWeek");
    //   updatedKeys.push_back("beginTime");
    //   updatedKeys.push_back("endTime");
    //   updatedKeys.push_back("userType");
    //   updatedKeys.push_back("lastUpdate");

    //   for (JsonObject x : doc["updated"].as<JsonArray>())
    //   {
    //     if (is_valid_json(x, updatedKeys))
    //     {
    //       Uid id = x["id"];
    //       const char *dayOfWeek = x["dayOfWeek"];
    //       const char *beginTime = x["beginTime"];   // time
    //       const char *endTime = x["endTime"];       // time
    //       const char *userType = x["userType"];     // "STUDENT" | "PROFESSOR" | "EMPLOYEE"
    //       const char *lastUpdate = x["lastUpdate"]; // datatime
          
    //       Schedule schedule = daoManager->scheduleDao.findById(id);
          
    //       if (schedule.isValid())
    //       {
    //         schedule.build(id, Utils::findEnumByValue(dayOfWeekNames, String(dayOfWeek)), Utils::stringToDatetime(beginTime), Utils::stringToDatetime(endTime), Utils::findEnumByValue(userTypeNames, String(userType)), Utils::stringToDatetime(lastUpdate));
    //         daoManager->scheduleDao.update(schedule);
    //         Serial.print("[LOG]: schedule updated: ");
    //         Serial.println(id);
    //       }
    //       else
    //       {
    //         schedule.build(id, Utils::findEnumByValue(dayOfWeekNames, String(dayOfWeek)), Utils::stringToDatetime(beginTime), Utils::stringToDatetime(endTime), Utils::findEnumByValue(userTypeNames, String(userType)), Utils::stringToDatetime(lastUpdate));
    //         daoManager->scheduleDao.save(schedule);
    //         Serial.print("[LOG]: schedule added: ");
    //         Serial.println(id);
    //       }
    //     }
    //     else
    //     {
    //       Serial.println("[WARN]: An invalid updated result was found!");
    //     }
    //   }
    // }
    // else
    // {
    //   Serial.println("[WARN]: Request result json was different of specification!");
    // }
  }

  bool updateEvents()
  {
    Serial.println("Updating events..");
    if (!this->networkGuard())
      return false;

    std::vector<String> rs;
    /** TODO rs.size() from type Vector was returning 0 */
    Event event;
    event.build(0, Utils::now(), EventType::OUT); // Create a dummy
    rs.push_back(event.toJSON()); // Add a dummy event to ensure at least one event is sent

    for(Event &event : daoManager->eventDao.findAll())
    {
      Serial.println("JSONING..");
      rs.push_back(event.toJSON());
    }

    int code = 204;
    Serial.printf("Updating events %d..", rs.size());
    if (rs.size())
    {
      code = this->sendUpdateEventsRequest(rs);

      if (code == 200){
        daoManager->eventDao.removeAll();
      }
    }

    return code == 200 || code == 204;
  }

  bool updateSchedules()
  {
    Serial.println("Updating schedules..");
    if (!this->networkGuard())
      return false;

    return this->sendUpdateScheduleRequest();
  }
  bool updateKeyrings()
  {
    Serial.println("Updating keyrings..");
    if (!this->networkGuard())
      return false;
    return this->sendUpdateKeyringsRequest();
  }

  void startupNetwork()
  {
    WiFi.setHostname("DACOM DOOR Esp32");
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->appConfig->config.wifiSSID.c_str(), this->appConfig->config.wifiPassword.c_str());

    int count = 0;
    int wifi_connection_max_tries = 10;
    // Waiting for wifi connection
    while (count < wifi_connection_max_tries && !WiFi.isConnected())
    {
      delay(1000);
      Serial.printf("Connecting to WiFi %s...%d\n", this->appConfig->config.wifiSSID, count);
      count++;
    }

    if (!WiFi.isConnected()) {
      Serial.println("Fail to connect to WiFi!");
    } else {
      Serial.println("WiFi connected!");
      this->updateDateTime();
    }

  }

public:
  boolean hasNetwork() { return WiFi.isConnected(); }

  AppTasks(AppConfig* appConfig): daoManager(&DaoManager::instance()), appConfig(appConfig){ }

  void startup()
  {
    this->startupNetwork();
    this->lastUpdateTry = Utils::now()-60; // Set last update try to 60 seconds ago to force an update on the first run
  }

  void run()
  {
    if(!this->networkGuard()) return;

    if(this->hasNetwork() && !this->timeUpdated)
        this->updateDateTime();

    // time_t represents the number of seconds from 1970 until now.
    time_t t = Utils::now();
    time_t diff = t - this->appConfig->config.lastUpdate;
    if (diff > this->appConfig->config.updateDelay && t - this->lastUpdateTry > 60)
    {
      this->lastUpdateTry = t; // Update last try time
      if(this->updateKeyrings())
      // if(this->updateKeyrings()&&
      //    this->updateSchedules() &&
      //    this->updateEvents())
      {
        this->appConfig->config.lastUpdate = t;
        this->appConfig->save();
        Serial.print("[LOG]: Last update: ");
        Serial.println(Utils::datetimeToString(t));
      }
      else
      {
        Serial.println("[ERROR]: Fail to update keyrings, schedules or events.");
      }
    }
    // esp_light_sleep_start();
  }
};

#endif