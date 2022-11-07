//#include <Arduino.h>
//#include "LittleFS.h"

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h>
#endif

//#include <ArduinoWebsockets.h>
//#include <ArduinoJson.h>

//#include <../include/wifi.cpp>
//#include <../lib/ocs_arduino_library/src/opencommunitysafety.cpp>
#include <opencommunitysafety.cpp>
//#include <../lib/ocs_arduino_library/src/WebServer.cpp>
#include <WebServer.cpp>
#include "AsyncJson.h"

//using namespace websockets;
using namespace ocs;

#ifdef ESP32
WiFiMulti wifiMulti;
#elif defined(ESP8266)
ESP8266WiFiMulti wifiMulti;
#endif

ocs::OpenCommunitySafety ocsClass;
ocs::LocalStore lstore;
WebAdmin ocsWebAdmin(80);

#ifdef ESP32
const char *echo_org_ssl_ca_cert = R"(-----BEGIN CERTIFICATE-----
MIIEDzCCAvegAwIBAgIBADANBgkqhkiG9w0BAQUFADBoMQswCQYDVQQGEwJVUzEl
MCMGA1UEChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMp
U3RhcmZpZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQw
NjI5MTczOTE2WhcNMzQwNjI5MTczOTE2WjBoMQswCQYDVQQGEwJVUzElMCMGA1UE
ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMpU3RhcmZp
ZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwggEgMA0GCSqGSIb3
DQEBAQUAA4IBDQAwggEIAoIBAQC3Msj+6XGmBIWtDBFk385N78gDGIc/oav7PKaf
8MOh2tTYbitTkPskpD6E8J7oX+zlJ0T1KKY/e97gKvDIr1MvnsoFAZMej2YcOadN
+lq2cwQlZut3f+dZxkqZJRRU6ybH838Z1TBwj6+wRir/resp7defqgSHo9T5iaU0
X9tDkYI22WY8sbi5gv2cOj4QyDvvBmVmepsZGD3/cVE8MC5fvj13c7JdBmzDI1aa
K4UmkhynArPkPw2vCHmCuDY96pzTNbO8acr1zJ3o/WSNF4Azbl5KXZnJHoe0nRrA
1W4TNSNe35tfPe/W93bC6j67eA0cQmdrBNj41tpvi/JEoAGrAgEDo4HFMIHCMB0G
A1UdDgQWBBS/X7fRzt0fhvRbVazc1xDCDqmI5zCBkgYDVR0jBIGKMIGHgBS/X7fR
zt0fhvRbVazc1xDCDqmI56FspGowaDELMAkGA1UEBhMCVVMxJTAjBgNVBAoTHFN0
YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xMjAwBgNVBAsTKVN0YXJmaWVsZCBD
bGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8w
DQYJKoZIhvcNAQEFBQADggEBAAWdP4id0ckaVaGsafPzWdqbAYcaT1epoXkJKtv3
L7IezMdeatiDh6GX70k1PncGQVhiv45YuApnP+yz3SFmH8lU+nLMPUxA2IGvd56D
eruix/U0F47ZEUD0/CwqTRV/p2JdLiXTAAsgGh1o+Re49L2L7ShZ3U0WixeDyLJl
xy16paq8U4Zt3VekyvggQQto8PT7dL5WXXp59fkdheMtlb71cZBDzI0fmgAKhynp
VSJYACPq4xJDKVtHCN2MQWplBqjlIapBtJUhlbl90TSrE9atvNziPTnNvT51cKEY
WQPJIrSPnNVeKtelttQKbfi3QBFGmh95DmK/D5fs4C8fF5Q=
-----END CERTIFICATE-----
)";

#elif defined(ESP8266)
const char *echo_org_ssl_ca_cert = "2A EE AF BB 00 2B 58 11 72 9E 1E 98 C8 8C C7 82 52 5A 37 E6";

#endif

String deviceId = "";

#ifdef ESP32

const int gpio_in_01 = 32;
const int gpio_out_01 = 21;

#elif defined(ESP8266)

const int gpio_in_01 = 0;
const int gpio_out_01 = 2;

#endif

const String websockets_server_host = "wss://open-community-safety.herokuapp.com/ws/device";

void cb(WiFiClient *client, String path, String method)
{
  Serial.println("Entra en el callback " + method);
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, F("text/plain"), "Not found");
}

void getSettings(AsyncWebServerRequest *request)
{
  String outputJson = "";
  serializeJson(ocsClass.toJson(), outputJson);
  Serial.println(outputJson);
  request->send(200, F("application/json"), outputJson);
}

OpenCommunitySafety::delegateSetup setupParameters = []() -> ocs::Config
{
  //  Serial.println("Obtener Configuración");

  ocs::Config c;
  // = ocs::OpenCommunitySafety::getLocalConfig();
  c.fromLocalStore();
  // c.caCert_fingerPrint = "2A EE AF BB 00 2B 58 11 72 9E 1E 98 C8 8C C7 82 52 5A 37 E6";

  c.wifi[1].ssid = "edwinspire";
  c.wifi[1].pwd = "Caracol1980";

  /*
    ocs::Config c;
    c.websoketHost = "wss://open-community-safety.herokuapp.com/ws/device";
    c.wifi[0].ssid = "ocsdefault";
    c.wifi[0].pwd = "ocs@qwerty";

    c.wifi[1].ssid = "edwinspire";
    c.wifi[1].pwd = "Caracol1980";

    c.wifi[2].ssid = "edwinspiremovil";
    c.wifi[2].pwd = "libp1983";

    c.deviceId = "00a0aa00-aa00-0000-0000-000000000000";
    c.caCert = "2A EE AF BB 00 2B 58 11 72 9E 1E 98 C8 8C C7 82 52 5A 37 E6";

    c.input->gpio = 0;
    c.output->gpio = 2;
  */

  return c;
};

OpenCommunitySafety::delegateSaveConfig saveParameters = [](ocs::Config config) -> void
{
  Serial.println(F("--->>> Guarda "));
  // ocs::OpenCommunitySafety::setLocalConfig(config);
  config.saveLocalStorage();
};

AsyncCallbackJsonWebHandler *handlerBody = new AsyncCallbackJsonWebHandler("/setsettings", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                           {
ocsClass.setFromJson(json);
    request->send(200, F("application/json"), "{}"); });

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10000);

  ocsWebAdmin.on("/getsettings", getSettings);
  // ocsWebAdmin.on("/setsettings", HTTP_POST, onRequest, onUpload, setSettings);
  ocsWebAdmin.addHandler(handlerBody); // Para poder leer el body enviado en el request

  ocsWebAdmin.onNotFound(notFound);
  ocsWebAdmin.setup();

  //  delay(10000);

  ocsClass.setup(setupParameters, saveParameters);

  // delay(5000);

  //  byte size = ocsClass.getMaxSSIDs();
  // wifiMulti.addAP("edwinspire", "Caracol1980");

  for (byte i = 0; i < ocs::MAX_SSID_WIFI; i = i + 1)
  {

    if (ocsClass.ConfigParameter.wifi[i].ssid.length() > 5)
    {
      Serial.println("Add SSID => " + ocsClass.ConfigParameter.wifi[i].ssid);
      wifiMulti.addAP(ocsClass.ConfigParameter.wifi[i].ssid.c_str(), ocsClass.ConfigParameter.wifi[i].pwd.c_str());
    }
  }

  Serial.println(F("Connecting Wifi..."));

  if (wifiMulti.run() == WL_CONNECTED)
  {
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    ocsWebAdmin.begin();

    ocsClass.connectWS();
  }
}

void loop()
{
  Serial.println(F("Loop principal!"));
  Serial.println(WiFi.localIP());
  // delay(3000);
  //  put your main code here, to run repeatedly:
  if (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.println(F("WiFi not connected!"));
    delay(1000);
  }
  ocsClass.loop();
}