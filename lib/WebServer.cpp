extern "C"
{
#include "c_types.h"
}
#include <Arduino.h>
#include "LittleFS.h"
#ifdef ESP32
// #include <WiFi.h>
// #include <AsyncTCP.h>
//  #include "SPIFFS.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>
#endif

namespace ocs
{

    BearSSL::ServerSessions serverCache(10);

    const char *MIME_TYPE_JSON = "application/json";

    // ESP8266WebServer server(port);
    static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSzCCAjMCCQD2ahcfZAwXxDANBgkqhkiG9w0BAQsFADCBiTELMAkGA1UEBhMC
VVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU9yYW5nZSBDb3VudHkx
EDAOBgNVBAoMB1ByaXZhZG8xGjAYBgNVBAMMEXNlcnZlci56bGFiZWwuY29tMR8w
HQYJKoZIhvcNAQkBFhBlYXJsZUB6bGFiZWwuY29tMB4XDTE4MDMwNjA1NDg0NFoX
DTE5MDMwNjA1NDg0NFowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3Rh
dGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAPVKBwbZ+KDSl40YCDkP6y8Sv4iNGvEOZg8Y
X7sGvf/xZH7UiCBWPFIRpNmDSaZ3yjsmFqm6sLiYSGSdrBCFqdt9NTp2r7hga6Sj
oASSZY4B9pf+GblDy5m10KDx90BFKXdPMCLT+o76Nx9PpCvw13A848wHNG3bpBgI
t+w/vJCX3bkRn8yEYAU6GdMbYe7v446hX3kY5UmgeJFr9xz1kq6AzYrMt/UHhNzO
S+QckJaY0OGWvmTNspY3xCbbFtIDkCdBS8CZAw+itnofvnWWKQEXlt6otPh5njwy
+O1t/Q+Z7OMDYQaH02IQx3188/kW3FzOY32knER1uzjmRO+jhA8CAwEAATANBgkq
hkiG9w0BAQsFAAOCAQEAnDrROGRETB0woIcI1+acY1yRq4yAcH2/hdq2MoM+DCyM
E8CJaOznGR9ND0ImWpTZqomHOUkOBpvu7u315blQZcLbL1LfHJGRTCHVhvVrcyEb
fWTnRtAQdlirUm/obwXIitoz64VSbIVzcqqfg9C6ZREB9JbEX98/9Wp2gVY+31oC
JfUvYadSYxh3nblvA4OL+iEZiW8NE3hbW6WPXxvS7Euge0uWMPc4uEcnsE0ZVG3m
+TGimzSdeWDvGBRWZHXczC2zD4aoE5vrl+GD2i++c6yjL/otHfYyUpzUfbI2hMAA
5tAF1D5vAAwA8nfPysumlLsIjohJZo4lgnhB++AlOg==
-----END CERTIFICATE-----
)EOF";

    static const char serverKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA9UoHBtn4oNKXjRgIOQ/rLxK/iI0a8Q5mDxhfuwa9//FkftSI
IFY8UhGk2YNJpnfKOyYWqbqwuJhIZJ2sEIWp2301OnavuGBrpKOgBJJljgH2l/4Z
uUPLmbXQoPH3QEUpd08wItP6jvo3H0+kK/DXcDzjzAc0bdukGAi37D+8kJfduRGf
zIRgBToZ0xth7u/jjqFfeRjlSaB4kWv3HPWSroDNisy39QeE3M5L5ByQlpjQ4Za+
ZM2yljfEJtsW0gOQJ0FLwJkDD6K2eh++dZYpAReW3qi0+HmePDL47W39D5ns4wNh
BofTYhDHfXzz+RbcXM5jfaScRHW7OOZE76OEDwIDAQABAoIBAQDKov5NFbNFQNR8
djcM1O7Is6dRaqiwLeH4ZH1pZ3d9QnFwKanPdQ5eCj9yhfhJMrr5xEyCqT0nMn7T
yEIGYDXjontfsf8WxWkH2TjvrfWBrHOIOx4LJEvFzyLsYxiMmtZXvy6YByD+Dw2M
q2GH/24rRdI2klkozIOyazluTXU8yOsSGxHr/aOa9/sZISgLmaGOOuKI/3Zqjdhr
eHeSqoQFt3xXa8jw01YubQUDw/4cv9rk2ytTdAoQUimiKtgtjsggpP1LTq4xcuqN
d4jWhTcnorWpbD2cVLxrEbnSR3VuBCJEZv5axg5ZPxLEnlcId8vMtvTRb5nzzszn
geYUWDPhAoGBAPyKVNqqwQl44oIeiuRM2FYenMt4voVaz3ExJX2JysrG0jtCPv+Y
84R6Cv3nfITz3EZDWp5sW3OwoGr77lF7Tv9tD6BptEmgBeuca3SHIdhG2MR+tLyx
/tkIAarxQcTGsZaSqra3gXOJCMz9h2P5dxpdU+0yeMmOEnAqgQ8qtNBfAoGBAPim
RAtnrd0WSlCgqVGYFCvDh1kD5QTNbZc+1PcBHbVV45EmJ2fLXnlDeplIZJdYxmzu
DMOxZBYgfeLY9exje00eZJNSj/csjJQqiRftrbvYY7m5njX1kM5K8x4HlynQTDkg
rtKO0YZJxxmjRTbFGMegh1SLlFLRIMtehNhOgipRAoGBAPnEEpJGCS9GGLfaX0HW
YqwiEK8Il12q57mqgsq7ag7NPwWOymHesxHV5mMh/Dw+NyBi4xAGWRh9mtrUmeqK
iyICik773Gxo0RIqnPgd4jJWN3N3YWeynzulOIkJnSNx5BforOCTc3uCD2s2YB5X
jx1LKoNQxLeLRN8cmpIWicf/AoGBANjRSsZTKwV9WWIDJoHyxav/vPb+8WYFp8lZ
zaRxQbGM6nn4NiZI7OF62N3uhWB/1c7IqTK/bVHqFTuJCrCNcsgld3gLZ2QWYaMV
kCPgaj1BjHw4AmB0+EcajfKilcqtSroJ6MfMJ6IclVOizkjbByeTsE4lxDmPCDSt
/9MKanBxAoGAY9xo741Pn9WUxDyRplww606ccdNf/ksHWNc/Y2B5SPwxxSnIq8nO
j01SmsCUYVFAgZVOTiiycakjYLzxlc6p8BxSVqy6LlJqn95N8OXoQ+bkwUux/ekg
gz5JWYhbD6c38khSzJb0pNXCo3EuYAVa36kDM96k1BtWuhRS10Q1VXk=
-----END RSA PRIVATE KEY-----
)EOF";

    const byte MAX_SESSIONS_SSE = 10;

    class OCSWebAdmin : public ESP8266WebServerSecure
    {

    private:
        BearSSL::ESP8266WebServerSecure::ClientType clients[ocs::MAX_SESSIONS_SSE];
        unsigned long previousMillisSendAlive = 0;
        void handleNotFound()
        {
            Serial.println(F("Handle not found"));
            String message = "Handle Not Found\n\n";
            message += "URI: ";
            message += this->uri();
            message += "\nMethod: ";
            message += (this->method() == HTTP_GET) ? "GET" : "POST";
            message += "\nArguments: ";
            message += this->args();
            message += "\n";
            for (uint8_t i = 0; i < this->args(); i++)
            {
                message += " " + this->argName(i) + ": " + this->arg(i) + "\n";
            }
            this->send(404, "text/plain", message);
        }

        void SSEHandler()
        {
            this->client().setNoDelay(true);
            this->client().setSync(true);
            this->setContentLength(CONTENT_LENGTH_UNKNOWN); // the payload can go on forever
            this->sendContent_P(PSTR("HTTP/1.1 200 OK\nContent-Type: text/event-stream;\nConnection: keep-alive\nCache-Control: no-cache\nAccess-Control-Allow-Origin: *\n\n"));
            // BearSSL::ESP8266WebServerSecure::ClientType cli = this->client();

            this->clients[0] = this->client();

            /*
            unsigned int i = 0;
            while (i < 10000)
            {

                if (this->clients[0].connected())
                {
                    this->clients[0].printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", \"%s\":{\"state\":%d, \"prevState\":%d}}\n\n"), this->clients[0].remoteIP().toString(), 10, 20);
                    this->clients[0].println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n"));       // Extra newline required by SSE standard
                    this->clients[0].println(F("message: event\ndata: { \"TYPE\":\"KEEP-ALIVE2222\" }\n")); // Extra newline required by SSE standard
                }
                else
                {
                    this->clients[0].flush();
                    this->clients[0].stop();
                    break;
                }

                delay(3000);
                i = i + 1;
            }
            */
        }
        void startServers()
        {
            if (!LittleFS.begin())
            {
                Serial.println(F("An Error has occurred while mounting LittleFS"));
            }

            this->enableCORS(true);

            this->on(F("/rest/events"), [&]()
                     { this->SSEHandler(); });
            this->serveStatic("/", LittleFS, "/build/", "max-age=31536000");
            this->onNotFound([&]()
                             { return this->handleNotFound(); });

            this->begin();
            Serial.println("HTTP server and  SSE EventSource started");
        }

        void SendAlive()
        {
            this->sendSSE("event", F("{\"TYPE\":\"KEEP-ALIVE\"}"));

            // this->clients[i].println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n")); // Extra newline required by SSE standard

            /*
            for (int i = 0; i < MAX_SESSIONS_SSE; i++)
            {
                if (this->clients[i].connected())
                {
                    // this->clients[i].printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", \"%s\":{\"state\":%d, \"prevState\":%d}}\n\n"), this->clients[0].remoteIP().toString(), 10, 20);
                    this->clients[i].println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n")); // Extra newline required by SSE standard
                    // this->clients[i].println(F("message: event\ndata: { \"TYPE\":\"KEEP-ALIVE2222\" }\n")); // Extra newline required by SSE standard
                }
                else
                {
                    this->clients[i].flush();
                    this->clients[i].stop();
                    break;
                }
            }
            */
        }

    public:
        OCSWebAdmin(uint16_t port = 443) : ESP8266WebServerSecure(port) {}

        void setup(void)
        {

            this->getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

            // Cache SSL sessions to accelerate the TLS handshake.
            this->getServer().setCache(&serverCache);

            startServers(); // start web and SSE servers
        }

        void loop(void)
        {
            // obtener el tiempo actual en milisegundos
            unsigned long currentMillis = millis();

            // verificar si ha pasado el tiempo suficiente desde la última ejecución del método
            if (currentMillis - this->previousMillisSendAlive >= 45000)
            {
                // guardar el tiempo actual como la última vez que se ejecutó el método
                this->previousMillisSendAlive = currentMillis;

                // ejecutar el método deseado aquí
                this->SendAlive();
            }
            this->handleClient();
            yield();
        }

        void sendSSE(String message_type, String data)
        {
            String msg = message_type + ": event\ndata: " + data + "\n";

            for (int i = 0; i < MAX_SESSIONS_SSE; i++)
            {
                if (this->clients[i].connected())
                {
                    this->clients[i].println(msg);
                    // this->clients[i].printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", \"%s\":{\"state\":%d, \"prevState\":%d}}\n\n"), this->clients[0].remoteIP().toString(), 10, 20);
                    // this->clients[i].println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n")); // Extra newline required by SSE standard
                    // this->clients[i].println(F("message: event\ndata: { \"TYPE\":\"KEEP-ALIVE2222\" }\n")); // Extra newline required by SSE standard
                }
                else
                {
                    this->clients[i].flush();
                    this->clients[i].stop();
                    break;
                }
            }
        }
    };

}
