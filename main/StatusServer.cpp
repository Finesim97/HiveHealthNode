#include "StatusServer.h"


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void StatusServer::begin(){
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
  server.begin();
}
