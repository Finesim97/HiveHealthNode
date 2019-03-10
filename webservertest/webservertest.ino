#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <pgmspace.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/events");

#define BAUDRATE 115200 // Serial Baudrate

const char index_html[] PROGMEM = "<html><head><script>var socket = new WebSocket(\"ws://\" + location.host + \"/events\");socket.onmessage = function (e) {console.log('Server: ' + e.data);};</script></head><body></body></html>"; 

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Connected %u", client->id());
    //client->ping();
  } else if(type == WS_EVT_DISCONNECT){
  } else if(type == WS_EVT_ERROR){
  } else if(type == WS_EVT_PONG){
  } else if(type == WS_EVT_DATA){
  }
}

void setup() {
  Serial.begin(BAUDRATE);
    WiFi.softAP("TEST");
   server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html);
  Serial.println("Request!");
  });
  server.onNotFound(notFound);
  //First request will return 0 results unless you start scan from somewhere else (loop/setup)
//Do not request more often than 3-5 seconds
server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
  String json = "[";
  int n = WiFi.scanComplete();
  if(n == -2){
    WiFi.scanNetworks(true);
  } else if(n){
    for (int i = 0; i < n; ++i){
      if(i) json += ",";
      json += "{";
      json += "\"rssi\":"+String(WiFi.RSSI(i));
      json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
      json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
      json += ",\"channel\":"+String(WiFi.channel(i));
      json += ",\"secure\":"+String(WiFi.encryptionType(i));
    //  json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
      json += "}";
    }
    WiFi.scanDelete();
    if(WiFi.scanComplete() == -2){
      WiFi.scanNetworks(true);
    }
  }
  json += "]";
  request->send(200, "application/json", json);
  json = String();
});
  server.begin();
      if (!MDNS.begin("esp32")) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
          MDNS.addService("http", "tcp", 80);
          Serial.println("Looping");
}

void loop() {
  if(millis()%1000==0){
  ws.printfAll("Time: %u\n",millis());
  }

}
