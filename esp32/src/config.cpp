#ifndef APP_CONFIG
#define APP_CONFIG

#include <Arduino.h>
#include <NimBLEDevice.h>

#include "models.cpp"
#include "daosqlite3.cpp"

class AppConfig
{
  DaoManager *daoManager = &DaoManager::instance();

  NimBLEServer *pServer;
  NimBLECharacteristic *pCharacteristic = nullptr;

public:
  Config config;

  void save()
  {
    this->daoManager->configDao.save(config);
  }

  void sendConfig(Config &config)
  {
    /** MTU Detaul is 256 bytes  */
    String configStr =
        Config::CONFIG_PASSWORD + "=" + config.configPassword + "\n" +
        Config::BOARD_VERSION + "=" + config.boardVersion + "\n" +
        Config::ROOM_NAME + "=" + config.roomName + "\n" +
        Config::LAST_UPDATE+ "=" + Utils::datetimeToString(config.lastUpdate) + "\n" +
        Config::RELAY_DELAY + "=" + String(config.relayDelay) + "\n" +
        Config::WIFI_SSID + "=" + config.wifiSSID + "\n" +
        Config::WIFI_PASSWORD + "=" + config.wifiPassword + "\n" +
        Config::GMT_ZONE + "=" + config.gmtZone + "\n" +
        Config::NEW_CONFIG_PASSWORD + "=" + config.newConfigPassword + "\n" +
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
      Serial.printf("Client address: %s\n", connInfo.getAddress().toString().c_str());

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
      Serial.printf("Client disconnected - start advertising\n");
      NimBLEDevice::startAdvertising();
    }

    void onMTUChange(uint16_t MTU, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
    }

    /********************* Security handled here *********************/
    uint32_t onPassKeyDisplay() override
    {
      Serial.printf("Server Passkey Display\n");
      /**
       * This should return a random 6 digit number for security
       *  or make your own static passkey as done here.
       */
      return 322661;
    }

    void onConfirmPassKey(NimBLEConnInfo &connInfo, uint32_t pass_key) override
    {
      Serial.printf("The passkey YES/NO number: %" PRIu32 "\n", pass_key);
      /** Inject false if passkeys don't match. */
      NimBLEDevice::injectConfirmPasskey(connInfo, true);
    }

    void onAuthenticationComplete(NimBLEConnInfo &connInfo) override
    {
      /** Check that encryption was successful, if not we disconnect the client */
      if (!connInfo.isEncrypted())
      {
        NimBLEDevice::getServer()->disconnect(connInfo.getConnHandle());
        Serial.printf("Encrypt connection failed - disconnecting client\n");
        return;
      }

      Serial.printf("Secured connection to: %s\n", connInfo.getAddress().toString().c_str());
    }
  };
  // Handler para receber comandos via BLE
  class ConfigBLECallbacks : public NimBLECharacteristicCallbacks
  {
    AppConfig *appConfig;
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("%s : onRead(), value: %s\n",
                    pCharacteristic->getUUID().toString().c_str(),
                    pCharacteristic->getValue().c_str());
    }

    bool checkPassword(const String &password, NimBLECharacteristic *pChar)
    {
      if(this->appConfig->config.configPassword != password)
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
      Serial.printf("[LOG] BLE.onWrite(), command: %s\n", incoming.c_str());
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
          if (this->checkPassword(configTemp.configPassword, pChar)){
            pChar->setValue("Rebooting...");
            pChar->notify();
            delay(1000); // Espera um pouco para enviar a mensagem
            ESP.restart();
         }
        }else
        if (incoming == "FORMAT")
        {
          if (this->checkPassword(configTemp.configPassword, pChar)){
            pChar->setValue("Formatting...");
            pChar->notify();
            SPIFFS.format();
            pChar->setValue("formatted.");
         }
        }else
        if (incoming == "GET_CONFIG")
        {
          if (this->checkPassword(configTemp.configPassword, pChar)){
            configTemp = this->appConfig->config;
            this->appConfig->sendConfig(configTemp);
          }
        }
        else if (incoming == "SAVE")
        {
          if (this->checkPassword(configTemp.configPassword, pChar))
          {
            this->appConfig->config = configTemp;
            if (!this->appConfig->config.newConfigPassword.isEmpty())
            {
              this->appConfig->config.configPassword = this->appConfig->config.newConfigPassword;
              this->appConfig->config.newConfigPassword = ""; // Clear the new password after saving
            }
            this->appConfig->save();
            pChar->setValue("Saved.");
          }
        }
        else
        {
          pChar->setValue("Invalid format. Use KEY=VALUE or GET_CONFIG");
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
    // Serial.println("SetupConfig...");
    config = this->daoManager->configDao.retrieve();
    setupBLE();
    // Serial.println("SetupConfig___...");
  }
};
#endif
