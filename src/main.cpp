#include <ArduinoJson.h> 
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Arduino.h>

// Dados da plataforma Konker
const char* KONKER_SERVER_URL = "data.demo.konkerlabs.net";
const int KONKER_SERVER_PORT = 80;
const char* USER = "";
const char* PWD = "";

// Dados da rede WiFi
const char * SSID = "";
const char * passwd = "";

ESP8266WiFiMulti wifiMulti;

int pubHTTP(String channel, const String & message);
String jsonRssi(int32_t rssi);

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(100);

  Serial.println();
  Serial.print("Core: ");
  Serial.println(ESP.getCoreVersion());
  Serial.print("SDK: ");
  Serial.println(ESP.getSdkVersion());

  WiFi.persistent(false);
  WiFi.disconnect();
  delay(10);
  WiFi.setAutoConnect(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setOutputPower(20.5f);
  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setAutoReconnect(true);

  wifiMulti.addAP(SSID, passwd);

  Serial.println("Connecting Wifi...");

  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(250);
    Serial.print('.');
  }
  
  Serial.print("\r\n\r\n\r\n");
  delay(1000);
}

void loop()
{
  if (wifiMulti.run() == WL_CONNECTED)
  {
    pubHTTP("rssi", jsonRssi(WiFi.RSSI()));
    Serial.println();
  }

  delay(5000);
}


int pubHTTP(String channel, const String & message)
{
  WiFiClient client;
  HTTPClient http;
  int httpCode = -1;

  String pub_dir = "/pub/";
  pub_dir += USER;
  pub_dir += "/" + channel;

  if (http.begin(client, KONKER_SERVER_URL, KONKER_SERVER_PORT, pub_dir))
  {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");
    http.setAuthorization(USER, PWD);
    httpCode = http.POST(message);

    if (httpCode == HTTP_CODE_OK)
    {
      // (void)http.getString();
      String payload = http.getString();
      Serial.print("HTTP resp: ");
      Serial.println(payload);
    }

    http.end();
  }

  return httpCode;
}

String jsonRssi(int32_t rssi)
{
  String output;
  StaticJsonDocument<200> jsonMSG;

  jsonMSG["rssi"] = rssi;
  serializeJson(jsonMSG, output);

  Serial.print("JSON: ");
  Serial.println(output);

  return output;
}
