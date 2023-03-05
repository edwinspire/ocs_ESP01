
#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
// #include <ESP8266WiFi.h>
#endif

#include <Interval.cpp>
#include <opencommunitysafety.cpp>

const uint32_t connectTimeoutMs = 10000;
using namespace ocs;

#ifdef ESP32
WiFiMulti wifiMulti;
#elif defined(ESP8266)
ESP8266WiFiMulti wifiMulti;
#endif

ocs::OpenCommunitySafety ocsClass;
edwinspire::Interval intervalConnectWiFi;

void wifi_reconnect()
{
  Serial.println(F("wifi_reconnect..."));
  Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("Está conectado... "));
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println(F("Connecting Wifi..."));
    if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED)
    {
      Serial.println(F("WiFi connected"));
      // Serial.println(F("IP address: "));
      // Serial.println(WiFi.localIP());
      // Serial.println(WiFi.SSID());
      ocsClass.ip = WiFi.localIP().toString();
      ocsClass.ssid = WiFi.SSID();
      ocsClass.begin();
      ocsClass.connect_websocket();
      //   ocsClass.connectWS();
    }
    // WiFi.disconnect();
  }
}

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  // delay(5000);
  ocsClass.setup();
  delay(5000);

  for (byte i = 0; i < ocs::MAX_SSID_WIFI; i = i + 1)
  {

    if (ocsClass.ConfigParameter.wifi[i].ssid.length() > 5)
    {
      Serial.println("Add SSID => " + ocsClass.ConfigParameter.wifi[i].ssid);
      wifiMulti.addAP(ocsClass.ConfigParameter.wifi[i].ssid.c_str(), ocsClass.ConfigParameter.wifi[i].pwd.c_str());
    }
  }

  wifi_reconnect();
  intervalConnectWiFi.setup(15000, &wifi_reconnect); // check wifi each 15 seconds
}

void loop()
{
  intervalConnectWiFi.loop();
  ocsClass.loop();
}