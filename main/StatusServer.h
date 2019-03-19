#if !defined _STATUSSERVER
#define _STATUSSERVER

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <pgmspace.h>


class EndableAsyncWebServer: public AsyncWebServer{
  using AsyncWebServer::AsyncWebServer;
  public:
    void end(){_server.end();}
};

class StatusServer{
    public:
      StatusServer(uint16_t port=80):server(port){};
      void begin();
      void log(const char* new_action);
    private:
      EndableAsyncWebServer server;
      AsyncWebSocket ws=AsyncWebSocket("/log");
      boolean ready=false;
};
#endif
