#ifdef ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFi.h>
#endif

#include <Interval.cpp>
#include <ArduinoWebsockets.h>
#include <opencommunitysafety.cpp>

using namespace websockets;
using namespace ocs;

#ifndef MAX_SSID_WIFI
MAX_SSID_WIFI = 4
#endif

#ifdef ESP32
    WiFiMulti wifiMulti;
#elif defined(ESP8266)
ESP8266WiFiMulti wifiMulti;
#endif

ocs::OpenCommunitySafety ocsClass;
edwinspire::Interval intervalConnectWiFi;
edwinspire::Interval intervalConnectWebSocket;

#ifdef ESP32

// const int gpio_in_01 = 32;
// const int gpio_out_01 = 21;

#elif defined(ESP8266)

const int gpio_in_01 = 0;
const int gpio_out_01 = 2;

#endif

void onMessageCallback(WebsocketsMessage message)
{
  Serial.print("-------------------------------> Got Message: ");
  // Serial.println(message.data());
  ocsClass.onMessageFromWebsocket(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data)
{
  if (event == WebsocketsEvent::ConnectionOpened)
  {
    Serial.println("-------------------------------> Connnection Opened");
  }
  else if (event == WebsocketsEvent::ConnectionClosed)
  {
    Serial.println("-------------------------------> Connnection Closed");
  }
  else if (event == WebsocketsEvent::GotPing)
  {
    Serial.println("-------------------------------> Got a Ping!");
  }
  else if (event == WebsocketsEvent::GotPong)
  {
    Serial.println("-------------------------------> Got a Pong!");
  }
}

WebsocketsClient client;

void setCertWebSocket()
{
  if (ocsClass.ConfigParameter.server.getSecure())
  {

    Serial.println("-----> setcert :");
    Serial.println(ocsClass.ConfigParameter.server.getcertificate().c_str());

#ifdef ESP32
    client.setCACert(ocsClass.ConfigParameter.server.getcertificate().c_str());
#elif defined(ESP8266)
    client.setFingerprint(ocsClass.ConfigParameter.server.getcertificate().c_str());

    Serial.println("setup_websocket: ");
    Serial.println(ocsClass.ConfigParameter.server.getcertificate().c_str());
#endif
  }
}

// Solo se debe ejecutar una vez
void setup_websocket()
{
  // run callback when messages are received
  client.onMessage(onMessageCallback);
  // run callback when events are occuring
  client.onEvent(onEventsCallback);

  ocsClass.onMessageToWebsocket([&](String msg)
                                { client.send(msg); });

  ocsClass.onPingIntervalWebSocket([&]()
                                   { Serial.println("Send ping...");
                                   client.ping(); });

  // La configuración de Authorization solo se debe hav¿cer una vez, ya que si se la vuelve a setear se van agregando mas lineas en lugar de reemplazarse
  client.addHeader("Authorization", ocsClass.getBasicAuthorizationString());
  setCertWebSocket();
}

void connect_websocket()
{

  //  Serial.println("====>>> connect_websocket");
  //  Serial.println(client.available());

  if (WiFi.status() == WL_CONNECTED && !client.available())
  {
    Serial.println("====>>> connect_websocket");

    Serial.println(ocsClass.ConfigParameter.server.getUrlServer());
    Serial.println(ocsClass.ConfigParameter.server.getcertificate());
    HttpWebsocketServer::freeMemory();
    setCertWebSocket();
    // client.setFingerprint(ocsClass.ConfigParameter.server.getcertificate().c_str());
    //  this->setUpCertificateFingerPrint();
    bool connected = client.connect(ocsClass.ConfigParameter.server.getUrlServer());

    if (connected)
    {
      Serial.println(F("WS Connected"));
      // client.send(HttpWebsocketServer::DynamicJsonToString(ocsClass.onConnect()));
      ocsClass.onConnect();
    }
    else
    {
      Serial.println(F("WS Not Connected!"));
      client.close();
    }
  }
}

void wifi_reconnect()
{

  Serial.println(F("wifi_reconnect..."));
  Serial.println(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("Está conectado... "));
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.localIP());
    // ocsClass.WifiConnected = true;
    ocsClass.setMACAddress(WiFi.macAddress());
    //    WiFi.macAddress();
    // connect_websocket();
  }
  else
  {
    // ocsClass.WifiConnected = false;
    Serial.println(F("Connecting Wifi..."));
    Serial.println(WiFi.status());
    if (wifiMulti.run() == WL_CONNECTED)
    {
      // ocsClass.WifiConnected = true;
      Serial.println(F("WiFi is connected"));

      ocsClass.ip = WiFi.localIP().toString();
      ocsClass.ssid = WiFi.SSID();
      ocsClass.begin();
      //  ocsClass.connect_websocket();
      //      connect_websocket();
    }
    // WiFi.disconnect();
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  ocsClass.setup();
  // ocsClass.WifiConnected = false;
  delay(5000);

  setup_websocket();

  Serial.println("Se termina de hacer el setup...");

  for (byte i = 0; i < MAX_SSID_WIFI; i = i + 1)
  {

    if (ocsClass.ConfigParameter.ssids[i].getSSID().length() > 5)
    {
      Serial.println("Add SSID => " + ocsClass.ConfigParameter.ssids[i].getSSID());
      wifiMulti.addAP(ocsClass.ConfigParameter.ssids[i].getSSID().c_str(), ocsClass.ConfigParameter.ssids[i].getPassword().c_str());
    }
  }

  wifi_reconnect();

  intervalConnectWiFi.setup(15000, &wifi_reconnect);        // check wifi each 15 seconds
  intervalConnectWebSocket.setup(5000, &connect_websocket); // check wifi each 16 seconds
}

void loop()
{
  intervalConnectWiFi.loop();
  ocsClass.loop();
  client.poll();
  intervalConnectWebSocket.loop();
}
