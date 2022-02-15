#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char *WIFI_SSID = "Vikings";
const char *WIFI_PASSWORD = "123muudar";

void setup()
{
  // Take some time to open up the Serial Monitor
  Serial.begin(115200);
  delay(1000);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() == WL_CONNECTED)
    Serial.println("Wifi Connected!\n");
  else
    Serial.printf("WiFi Failed!\n");

  if (WiFi.isConnected())
  {
    HTTPClient http;

    http.begin("https://community-open-weather-map.p.rapidapi.com/weather");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }
    else
    {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
}

void loop() {}
