#ifndef APP_CONFIG
#define APP_CONFIG

#include <Arduino.h>
#include <BluetoothSerial.h>

#include "models.cpp"
#include "daosqlite3.cpp"

static BluetoothSerial SerialBT;

static ConfigDao configDao;
static Config config;

static void startupConfig(){
    config = configDao.retrieve();
}

static void sendConfig(Config &config) {
  SerialBT.println(Config::CONFIG_PASSWORD + "=" + config.configPassword);
  SerialBT.println(Config::BOARD_VERSION + "=" + config.boardVersion);
  SerialBT.println(Config::SERVER_URL + "=" + config.serverURL);
  SerialBT.println(Config::ROOM_NAME + "=" + config.roomName);
  SerialBT.println(Config::UPDATE_DELAY + "=" + String(config.updateDelay));
  SerialBT.println(Config::RELAY_DELAY + "=" + String(config.relayDelay));
  SerialBT.println(Config::DOOR_OPENED_ALERT_DELAY + "=" + String(config.doorOpenedAlertDelay));
  SerialBT.println(Config::WIFI_SSID + "=" + config.wifiSSID);
  SerialBT.println(Config::WIFI_PASSWORD + "=" + config.wifiPassword);
}

static void handleBluetoothInput() {
  static String incoming = "";
  static Config config_;
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') {
      incoming.trim();
      int sep = incoming.indexOf('=');
      if (sep > 0) {
        String key = incoming.substring(0, sep);
        String value = incoming.substring(sep + 1);
        config_.applyConfig(key, value);
        SerialBT.println("OK");
      } else {
        if(incoming== "GET_CONFIG"){
            if(config.configPassword == config_.configPassword)
                sendConfig(config);
            else
                SerialBT.println("Wrong password.");
        } else if(incoming== "SAVE"){
            if(config.configPassword == config_.configPassword){
                config = config_;
                configDao.save(config);
            }
            else
                SerialBT.println("Wrong password.");
        } else {
            SerialBT.println("Invalid format. Use KEY=VALUE or GET_CONFIG");
        }
      }
      incoming = "";
    } else {
      incoming += c;
    }
  }
}

#endif