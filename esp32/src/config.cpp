#ifndef APP_CONFIG
#define APP_CONFIG

#include <Arduino.h>
#include <NimBLEDevice.h>

#include "models.cpp"
#include "daosqlite3.cpp"

// class ConfigBLECallbacks : public NimBLECharacteristicCallbacks {
//   void onWrite(NimBLECharacteristic* pChar) override {
//     std::string value = pChar->getValue();
//     static Config config_;
//     if (value.length() == 0) return;
//     if (value.find("GET_CONFIG") != std::string::npos) {
//       if (config.configPassword == config_.configPassword)
//         sendConfig(config);
//       else
//         pChar->setValue("Wrong password.\n");
//     } else if (value.find("SAVE") != std::string::npos) {
//       if (config.configPassword == config_.configPassword) {
//         config = config_;
//         configDao.save(config);
//         pChar->setValue("OK\n");
//       } else {
//         pChar->setValue("Wrong password.\n");
//       }
//     } else {
//       size_t sep = value.find('=');
//       if (sep != std::string::npos) {
//         std::string key = value.substr(0, sep);
//         std::string val = value.substr(sep + 1);
//         config_.applyConfig(key.c_str(), val.c_str());
//         pChar->setValue("OK\n");
//       } else {
//         pChar->setValue("Invalid format. Use KEY=VALUE or GET_CONFIG\n");
//       }
//     }
//   }
// };

class AppConfig
{
  // static NimBLEServer* pServer = nullptr;
  ConfigDao configDao;
  
  NimBLEServer *pServer;
  NimBLECharacteristic* pCharacteristic = nullptr;

public:
  Config config;

  void save() {
    configDao.save(config);
  }

  void sendConfig(Config &config)
  {
    String configStr =
    Config::CONFIG_PASSWORD + "=" + config.configPassword + "\n" +
      Config::BOARD_VERSION + "=" + config.boardVersion + "\n" +
      Config::SERVER_URL + "=" + config.serverURL + "\n" +
      Config::ROOM_NAME + "=" + config.roomName + "\n" +
      Config::UPDATE_DELAY + "=" + String(config.updateDelay) + "\n" +
      Config::RELAY_DELAY + "=" + String(config.relayDelay) + "\n" +
      Config::DOOR_OPENED_ALERT_DELAY + "=" + String(config.doorOpenedAlertDelay) + "\n" +
      Config::WIFI_SSID + "=" + config.wifiSSID + "\n" +
      Config::WIFI_PASSWORD + "=" + config.wifiPassword + "\n";
    if (pCharacteristic) {
      pCharacteristic->setValue(configStr.c_str());
      pCharacteristic->notify();
    }
  }

  // Handler para receber comandos via BLE
  class ConfigBLECallbacks : public NimBLECharacteristicCallbacks {
    AppConfig *appConfig;
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override
    {
      Serial.printf("%s : onRead(), value: %s\n",
                    pCharacteristic->getUUID().toString().c_str(),
                    pCharacteristic->getValue().c_str());
    }

    void onWrite(NimBLECharacteristic* pChar) {
      std::string value = pChar->getValue();
      String incoming = String(value.c_str());
      incoming.trim();
      int sep = incoming.indexOf('=');
      static Config configTemp; // Usar uma instância estática global pode causar problemas, mas aqui é seguro pois BLE callbacks são single-threaded
      if (sep > 0) {
        String key = incoming.substring(0, sep);
        String val = incoming.substring(sep + 1);
        configTemp.applyConfig(key, val);
        pChar->setValue("OK");
        pChar->notify();
      } else {
        if (incoming == "GET_CONFIG") {
          if (this->appConfig->config.configPassword == configTemp.configPassword)
            this->appConfig->sendConfig(this->appConfig->config);
          else {
            pChar->setValue("Wrong password.");
            pChar->notify();
          }
        } else if (incoming == "SAVE") {
          if (this->appConfig->config.configPassword == configTemp.configPassword) {
            this->appConfig->config = configTemp;
            this->appConfig->save();
            pChar->setValue("Saved.");
          } else {
            pChar->setValue("Wrong password.");
          }
          pChar->notify();
        } else {
          pChar->setValue("Invalid format. Use KEY=VALUE or GET_CONFIG");
          pChar->notify();
        }
      }
    }


    /**
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
        Serial.printf("Notification/Indication return code: %d, %s\n", code, NimBLEUtils::returnCodeToString(code));
    }

    /** Peer subscribed to notifications/indications */
    // void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
    //     std::string str  = "Client ID: ";
    //     str             += connInfo.getConnHandle();
    //     str             += " Address: ";
    //     str             += connInfo.getAddress().toString();
    //     if (subValue == 0) {
    //         str += " Unsubscribed to ";
    //     } else if (subValue == 1) {
    //         str += " Subscribed to notifications for ";
    //     } else if (subValue == 2) {
    //         str += " Subscribed to indications for ";
    //     } else if (subValue == 3) {
    //         str += " Subscribed to notifications and indications for ";
    //     }
    //     str += std::string(pCharacteristic->getUUID());

    //     Serial.printf("%s\n", str.c_str());
    // }

  public:
    ConfigBLECallbacks(AppConfig *appConfig):appConfig(appConfig) {}
  };
  
  /**  None of these are required as they will be handled by the library with defaults. **
   **                       Remove as you see fit for your needs                        */

  void setupBLE()
  {
    NimBLEDevice::init("ConfigBLE");
    pServer = NimBLEDevice::createServer();
  NimBLEService* pService = pServer->createService("00001801-0000-1000-8000-00805f9b34fb");
  pCharacteristic = pService->createCharacteristic(
    "00002a05-0000-1000-8000-00805f9b34fb", // UUID for the characteristic
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ_ENC | WRITE_ENC
  );
  pCharacteristic->setCallbacks(new ConfigBLECallbacks(this));
  pService->start();
  NimBLEDevice::getAdvertising()->setName("DACOM Door v1");
  NimBLEDevice::getAdvertising()->enableScanResponse(true);
  NimBLEDevice::getAdvertising()->addServiceUUID(pService->getUUID());
  NimBLEDevice::startAdvertising();
  }

  void startup()
  {
    // Serial.println("SetupConfig...");
    config = configDao.retrieve();
    setupBLE();
    // Serial.println("SetupConfig___...");
  }
};
#endif

