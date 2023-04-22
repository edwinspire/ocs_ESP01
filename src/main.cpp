
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "LittleFS.h"
#include "../lib/WebServer.cpp"

#ifndef STASSID
#define STASSID "edwinspire"
#define STAPSK "Caracol1980"
#endif

ocs::OCSWebAdmin webAdmin;

const char *ssid = STASSID;
const char *password = STAPSK;

void setup(void)
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.printf_P(PSTR("\nConnected to %s with IP address: %s\n"), ssid, WiFi.localIP().toString().c_str());

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  webAdmin.setup();
}

void loop(void)
{
  webAdmin.loop();
  MDNS.update();
  yield();
}
