#if !defined _STATUSSERVER
#define _STATUSSERVER

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <pgmspace.h>

class StatusServer{
    public:
      StatusServer(uint16_t port=80):server(port){};
      void begin();
      void log(const char* new_action);
    private:
      AsyncWebServer server;
      AsyncWebSocket ws=AsyncWebSocket("/log");
      boolean ready=false;
};
#endif
