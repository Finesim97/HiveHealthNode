#if !defined _STATUSSERVER
#define _STATUSSERVER

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

class StatusServer{
    public:
      StatusServer(uint16_t port=80):server(port){}
      void begin();
    private:
      AsyncWebServer server;
};
#endif
