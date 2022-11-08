#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#endif

#include <opencommunitysafety.cpp>

// using namespace websockets;
using namespace ocs;

#ifdef ESP32
WiFiMulti wifiMulti;
#elif defined(ESP8266)
ESP8266WiFiMulti wifiMulti;
#endif

ocs::OpenCommunitySafety ocsClass;
/*
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
*/

#ifdef ESP32

const int gpio_in_01 = 32;
const int gpio_out_01 = 21;

#elif defined(ESP8266)

const int gpio_in_01 = 0;
const int gpio_out_01 = 2;

#endif

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10000);
  ocsClass.setup();

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
    ocsClass.begin();
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