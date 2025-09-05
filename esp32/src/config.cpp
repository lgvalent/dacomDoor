#ifndef APP_CONFIG
#define APP_CONFIG

#define FW_VERSION "1.2.4"
#define VERSION_URL "http://door.valentin.com.br/firmware.info"
#define FIRMWARE_URL "http://door.valentin.com.br/firmware.bin"

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

#include "models.cpp"
#include "daosqlite3.cpp"

class AppConfig
{
  DaoManager *daoManager = &DaoManager::instance();

  NimBLEServer *pServer;
  NimBLECharacteristic *pCharacteristic = nullptr;

  AsyncWebServer *httpServer = nullptr;
  const char *SESSION_COOKIE = "auth";
  int sessionId;

public:
  inline static const String BUILD_INFO = String(FW_VERSION);

  Config config;

  bool checkForUpdate(bool performUpdate = false)
  {
    HTTPClient http;
    http.begin(VERSION_URL);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
      String newVersion = http.getString();
      newVersion.trim();
      Serial.printf("[CONF] Current version: %s\n", AppConfig::BUILD_INFO.c_str());
      Serial.printf("[CONF] Server version: %s\n", newVersion.c_str());

      if (newVersion != AppConfig::BUILD_INFO)
      {
        Serial.println("[CONF] New version found!");
        if (performUpdate)
          this->performUpdate();
        return true;
      }
      else
      {
        Serial.println("[CONF] No new version found.");
      }
    }
    else
    {
      Serial.printf("[CONF] Fail getting new version: %d\n", httpCode);
    }
    return false;
  }

  bool performUpdate()
  {
    Serial.println("[CONF] Update started...");

    HTTPClient http;

    http.begin(FIRMWARE_URL);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
      Serial.printf("[CONF] Fail downloading firmware: %d\n", httpCode);
      return false;
    }

    int contentLength = http.getSize();
    if (contentLength <= 0)
    {
      Serial.println("[CONF] Firmware invalid or unknow size.");
      http.end();
      return false;
    }

    if (!Update.begin(contentLength))
    {
      Serial.println("[CONF] Fail starting update.");
      Serial.printf("Error Update.begin(): %s\n", Update.errorString());
      Serial.printf("Code: %d\n", Update.getError());
      return false;
    }

    WiFiClient *stream = http.getStreamPtr();
    size_t written = Update.writeStream(*stream);

    if (written == contentLength && Update.end())
    {
      if (Update.isFinished())
      {
        Serial.println("[CONF] Update finished. Rebooting...");
        ESP.restart();
        return true;
      }
      else
      {
        Serial.println("[CONF] Update not finished.");
      }
    }
    else
    {
      Serial.printf("[CONF] Update error. Bytes writyen: %u\n", written);
    }
    http.end();
    return false;
  }

  void setupHttpServer()
  {
    if (httpServer)
      return; // Already started

    httpServer = new AsyncWebServer(80);

    // Login page
    httpServer->on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
                   {
      String html = "<html><head><title>Login</title><link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css'></head><body>";
      html += "<h2>Login</h2>";
      html += "<form action='/login' method='POST'>";
      html += "Senha: <input type='password' name='password'><br>";
      html += "<input type='submit' value='Login'>";
      html += "</form></body></html>";
      request->send(200, "text/html", html); });

    // Login submit
    httpServer->on("/login", HTTP_POST, [this](AsyncWebServerRequest *request)
                   {
      if (request->hasParam("password", true)) {
        String pass = request->getParam("password", true)->value();
        if (pass == this->config.configPassword) {
          // Login OK → define cookie e redireciona
          this->sessionId = random(100000, 999999);
          AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
          response->addHeader("Set-Cookie", String(SESSION_COOKIE) + "=" + String(this->sessionId) + "; Path=/");
          response->addHeader("Location", "/config");
          request->send(response);
        } else {
          request->send(403, "text/html", "<h3>Wrong password</h3><a href='/'>Try again</a>");
        }
      } else {
        request->send(400, "text/plain", "Password not found");
      } });

    // Config page, with authentication (by cookie)
    httpServer->on("/config", HTTP_GET, [this](AsyncWebServerRequest *request)
                   {
      if (!isAuthenticated(request)) {
        request->redirect("/");
        return;
      }
      String html = "<html><head><title>Configuração</title><link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/@picocss/pico@2/css/pico.min.css'></head><body>";
      html += "<h2>Device config</h2>";
      html += "<form action='/save' method='POST'>";
      html += "Config Password: <input type='text' name='configPassword' value='" + this->config.configPassword + "'><br>";
      html += "Board Version: <input type='text' name='boardVersion' value='" + String(this->config.boardVersion) + "'><br>";
      html += "Room Name: <input type='text' name='roomName' value='" + this->config.roomName + "'><br>";
      html += "Relay Delay: <input type='number' name='relayDelay' value='" + String(this->config.relayDelay) + "'><br>";
      html += "WiFi SSID: <input type='text' name='wifiSSID' value='" + this->config.wifiSSID + "'><br>";
      html += "WiFi Password: <input type='text' name='wifiPassword' value='" + this->config.wifiPassword + "'><br>";
      html += "GMT Zone: <input type='text' name='gmtZone' value='" + String(this->config.gmtZone) + "'><br>";
      html += "New Config Password: <input type='text' name='newConfigPassword' value='" + this->config.newConfigPassword + "'><br>";
      html += "Build Info: " + AppConfig::BUILD_INFO + "<br>";
      html += "Server URL: " + this->config.serverURL + "<br>";
      html += "Update Delay: " + String(this->config.updateDelay) + "<br>";
      html += "Last update: " + Utils::datetimeToString(this->config.lastUpdate) + "<br>";
      html += "MAC Addres: " + WiFi.macAddress() + "<br>";
      html += "<input type='submit' value='Save'>";
      html += "</form>";
      html += "<form action='/reboot' method='GET'>";
      html += "<input type='submit' value='Reboot'>";
      html += "</form>";
      html += "<form action='/format' method='GET'>";
      html += "<input type='submit' value='Format'>";
      html += "</form></body></html>";
      request->send(200, "text/html", html); });

    httpServer->on("/save", HTTP_POST, [this](AsyncWebServerRequest *request)
                   {
      if (!isAuthenticated(request)) {
        request->redirect("/");
        return;
      }
      int params = request->params();
      for (int i = 0; i < params; i++) {
        const AsyncWebParameter* p = request->getParam(i);
        String key = p->name();
        String value = p->value();
        if (key == "configPassword") this->config.configPassword = value;
        else if (key == "boardVersion") this->config.boardVersion = value.toInt();
        else if (key == "roomName") this->config.roomName = value;
        else if (key == "relayDelay") this->config.relayDelay = value.toInt();
        else if (key == "wifiSSID") this->config.wifiSSID = value;
        else if (key == "wifiPassword") this->config.wifiPassword = value;
        else if (key == "gmtZone") this->config.gmtZone = value.toInt();
        else if (key == "serverURL") this->config.serverURL = value;
        else if (key == "updateDelay") this->config.updateDelay = value.toInt();
        else if (key == "newConfigPassword") this->config.configPassword = value;
      }
      save();
      request->send(200, "text/html", "<h2>Settings saved!</h2><a href='/config'>Back</a>"); });

    httpServer->on("/reboot", HTTP_GET, [this](AsyncWebServerRequest *request)
                   {
      if (!isAuthenticated(request)) {
        request->redirect("/");
        return;
      }
      request->send(200, "text/html", "<h2>Rebooting!</h2><a href='/config'>Back</a>");
      this->reboot(); });

    httpServer->on("/format", HTTP_GET, [this](AsyncWebServerRequest *request)
                   {
      if (!isAuthenticated(request)) {
        request->redirect("/");
        return;
      }
      request->send(200, "text/html", "<h2>Formating!</h2><a href='/config'>Back</a>");
      this->format(); });

    delay(10000);
    httpServer->begin();
    Serial.println("[CONF] HTTP server started at port 80");
  }

  bool isAuthenticated(AsyncWebServerRequest *request)
  {
    if (request->hasHeader("Cookie"))
    {
      String cookie = request->header("Cookie");
      if (cookie.indexOf(String(SESSION_COOKIE) + "=" + String(this->sessionId)) != -1)
      {
        return true;
      }
    }
    return false;
  }
  void save()
  {
    if (!this->config.newConfigPassword.isEmpty())
    {
      this->config.configPassword = this->config.newConfigPassword;
      this->config.newConfigPassword = ""; // Clear the new password after saving
    }
    this->daoManager->configDao.save(config);
  }

  void reboot()
  {
    ESP.restart();
  }

  void format()
  {
    SPIFFS.format();
  }

  void sendConfig(Config &config)
  {
    /** MTU Detaul is 256 bytes  */
    String configStr =
        Config::CONFIG_PASSWORD + "=" + config.configPassword + "\n" +
        Config::BOARD_VERSION + "=" + config.boardVersion + "\n" +
        Config::ROOM_NAME + "=" + config.roomName + "\n" +
        Config::LAST_UPDATE + "=" + Utils::datetimeToString(config.lastUpdate) + "\n" +
        Config::RELAY_DELAY + "=" + String(config.relayDelay) + "\n" +
        Config::WIFI_SSID + "=" + config.wifiSSID + "\n" +
        Config::WIFI_PASSWORD + "=" + config.wifiPassword + "\n" +
        Config::GMT_ZONE + "=" + config.gmtZone + "\n" +
        Config::NEW_CONFIG_PASSWORD + "=" + config.newConfigPassword + "\n";
    if (pCharacteristic)
    {
      pCharacteristic->setValue(configStr.c_str());
    }
  }

  void sendInfo(Config &config)
  {
    /** MTU Detaul is 256 bytes  */
    String configStr =
        "VERSION=" + AppConfig::BUILD_INFO + "\n" +
        "MAC_ADDRESS=" + WiFi.macAddress() + "\n" +
        "IP_ADDRESS=" + WiFi.localIP().toString() + "\n" +
        Config::SERVER_URL + "=" + config.serverURL + "\n" +
        Config::UPDATE_DELAY + "=" + String(config.updateDelay) + "\n" +
        Config::DOOR_OPENED_ALERT_DELAY + "=" + String(config.doorOpenedAlertDelay) + "\n" +
        "* Non-persistent property\n";
    if (pCharacteristic)
    {
      pCharacteristic->setValue(configStr.c_str());
    }
  }

  class ServerCallbacks : public NimBLEServerCallbacks
  {
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("[CONF] Client address: %s\n", connInfo.getAddress().toString().c_str());

      /**
       *  We can use the connection handle here to ask for different connection parameters.
       *  Args: connection handle, min connection interval, max connection interval
       *  latency, supervision timeout.
       *  Units; Min/Max Intervals: 1.25 millisecond increments.
       *  Latency: number of intervals allowed to skip.
       *  Timeout: 10 millisecond increments.
       */
      pServer->updateConnParams(connInfo.getConnHandle(), 24, 48, 0, 180);
    }

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
    {
      Serial.printf("[CONF] Client disconnected - start advertising\n");
      NimBLEDevice::startAdvertising();
    }

    void onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("[CONF] MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
    }

    /********************* Security handled here *********************/
    uint32_t onPassKeyDisplay() override
    {
      Serial.printf("[CONF] Server Passkey Display\n");
      /**
       * This should return a random 6 digit number for security
       *  or make your own static passkey as done here.
       */
      return 322661;
    }

    void onConfirmPassKey(NimBLEConnInfo &connInfo, uint32_t pass_key) override
    {
      Serial.printf("[CONF] The passkey YES/NO number: %" PRIu32 "\n", pass_key);
      /** Inject false if passkeys don't match. */
      NimBLEDevice::injectConfirmPasskey(connInfo, true);
    }

    void onAuthenticationComplete(NimBLEConnInfo &connInfo) override
    {
      /** Check that encryption was successful, if not we disconnect the client */
      if (!connInfo.isEncrypted())
      {
        NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
        Serial.printf("[CONF] Encrypt connection failed - disconnecting client\n");
        return;
      }

      Serial.printf("[CONF] Secured connection to: %s\n", connInfo.getAddress().toString().c_str());
    }
  };
  // Handler para receber comandos via BLE
  class ConfigBLECallbacks : public NimBLECharacteristicCallbacks
  {
    AppConfig *appConfig;
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("[CONF] %s : onRead(), value: %s\n",
                    pCharacteristic->getUUID().toString().c_str(),
                    pCharacteristic->getValue().c_str());
    }

    bool checkPassword(const String &password, NimBLECharacteristic *pChar)
    {
      if (this->appConfig->config.configPassword != password)
      {
        pChar->setValue("Wrong password.");
        return false;
      }
      return true;
    }

    void onWrite(NimBLECharacteristic *pChar, NimBLEConnInfo &connInfo) override
    {
      std::string value = pChar->getValue();
      String incoming = String(value.c_str());
      incoming.trim();
      Serial.printf("[CONF] BLE.onWrite(), command: %s\n", incoming.c_str());
      int sep = incoming.indexOf('=');
      static Config configTemp; // Usar uma instância estática global pode causar problemas, mas aqui é seguro pois BLE callbacks são single-threaded
      if (sep > 0)
      {
        String key = incoming.substring(0, sep);
        String val = incoming.substring(sep + 1);
        configTemp.applyConfig(key, val);
        pChar->setValue("OK");
      }
      else
      {
        if (incoming == "REBOOT")
        {
          if (this->checkPassword(configTemp.configPassword, pChar))
          {
            pChar->setValue("Rebooting...");
            pChar->notify();
            delay(1000); // Espera um pouco para enviar a mensagem
            this->appConfig->reboot();
          }
        }
        else if (incoming == "FORMAT")
        {
          if (this->checkPassword(configTemp.configPassword, pChar))
          {
            pChar->setValue("Formatting...");
            pChar->notify();
            this->appConfig->format();
            pChar->setValue("formatted.");
          }
        }
        else if (incoming == "CONFIG")
        {
          if (this->checkPassword(configTemp.configPassword, pChar))
          {
            configTemp = this->appConfig->config;
            this->appConfig->sendConfig(configTemp);
          }
        } 
        else if (incoming == "INFO")
        {
          this->appConfig->sendInfo(this->appConfig->config);
        }
        else if (incoming == "SAVE")
        {
          if (this->checkPassword(configTemp.configPassword, pChar))
          {
            this->appConfig->config = configTemp;
            this->appConfig->save();
            pChar->setValue("Saved.");
          }
        }
        else
        {
          pChar->setValue("Invalid format. Use KEY=VALUE or CONFIG");
        }
      }
      pChar->notify();
    }

  public:
    ConfigBLECallbacks(AppConfig *appConfig) : appConfig(appConfig) {}
  };

  /**  None of these are required as they will be handled by the library with defaults. **
   **                       Remove as you see fit for your needs                        */

  void setupBLE()
  {
    NimBLEDevice::init("ConfigBLE");
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    NimBLEService *pService = pServer->createService("DAC0");
    pCharacteristic = pService->createCharacteristic(
        "D005", // UUID for the characteristic
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);
    pCharacteristic->setCallbacks(new ConfigBLECallbacks(this));
    pService->start();
    NimBLEDevice::getAdvertising()->setName(String("DACOM Door-" + this->config.roomName).c_str());
    NimBLEDevice::getAdvertising()->enableScanResponse(true);
    NimBLEDevice::getAdvertising()->addServiceUUID(pService->getUUID());
    NimBLEDevice::startAdvertising();
  }

  void startup()
  {
    config = this->daoManager->configDao.retrieve();
    setupBLE();
  }
};
#endif
