#if !defined _SETUPSERVER
#define _SETUPSERVER
#define NUMBERS "numbers"
#define STRINGS "strings"
#define BOOLS "booleans"
#define SENSORNAME "sensors"

#include "MQTTService.h"
#include "Sensors.h"
#include "WiFiStuff.h"
#include "ArduinoJson.h"
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <pgmspace.h>


class EndableAsyncWebServer: public AsyncWebServer{
  using AsyncWebServer::AsyncWebServer;
  public:
    void end(){_server.end();}
};

class SetupServer{
    public:
      SetupServer(uint16_t port=80):server(port){};
      void begin();
      void log(const char* new_action);
    private:
      EndableAsyncWebServer server;
      AsyncWebSocket ws=AsyncWebSocket("/log");
      boolean ready=false;
};
void getCurrentConfigJSON(Preferences &p);
boolean setConfigJSON(Preferences &p, char* jsonstr);

#endif
