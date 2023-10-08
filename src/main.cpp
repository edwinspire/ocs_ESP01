#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#endif

#include <Interval.cpp>
#include <opencommunitysafety.cpp>

using namespace ocs;

#ifdef ESP32
WiFiMulti wifiMulti;
#elif defined(ESP8266)
ESP8266WiFiMulti wifiMulti;
#endif

ocs::OpenCommunitySafety ocsClass;
edwinspire::Interval intervalConnectWiFi;

// const uint32_t connectTimeoutMs = 10000;

#ifdef ESP32

// const int gpio_in_01 = 32;
// const int gpio_out_01 = 21;

#elif defined(ESP8266)

const int gpio_in_01 = 0;
const int gpio_out_01 = 2;

#endif

void wifi_reconnect()
{
  Serial.println(F("wifi_reconnect..."));
  Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("Está conectado... "));
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
    ocsClass.WifiConnected = true;
  }
  else
  {
    ocsClass.WifiConnected = false;
    Serial.println(F("Connecting Wifi..."));
    Serial.println(WiFi.status());
    if (wifiMulti.run() == WL_CONNECTED)
    {
      ocsClass.WifiConnected = true;
      Serial.println(F("WiFi is connected"));

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


  Serial.println("Json de configuración");
  Serial.println(HttpWebsocketServer::DynamicJsonToString(ocsClass.ConfigParameter.toJson()));

  delay(5000);

  for (byte i = 0; i < ocs::MAX_SSID_WIFI; i = i + 1)
  {

    if (ocsClass.ConfigParameter.ssids[i].getSSID().length() > 5)
    {
      Serial.println("Add SSID => " + ocsClass.ConfigParameter.ssids[i].getSSID());
      wifiMulti.addAP(ocsClass.ConfigParameter.ssids[i].getSSID().c_str(), ocsClass.ConfigParameter.ssids[i].getPassword().c_str());
    }
  }

  Serial.println("Wifis agregadas...");
  delay(5000);

  wifi_reconnect();
  intervalConnectWiFi.setup(30000, &wifi_reconnect); // check wifi each 15 seconds
}

void loop()
{
  intervalConnectWiFi.loop();
  ocsClass.loop();
}
