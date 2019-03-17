#include "StatusServer.h"

//Unix command to prepare code: sed 's#\\#\\\\#g' | sed 's#"#\\"#g'
const char index_html[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"utf-8\"> <title>Node 6EF678 Status</title> <style>/* Deep Lemon (#F3C622), Dark Gunmetal (#23212C), Yellow Orange (#FCB43A), Onyx (#3A3637), Metallic Yellow (#FCD615) and Rufous (#992409). */ html, body{height: 100%; margin: 12px}body{display: flex; flex-flow: column; margin: auto; font-family: Verdana, Helvetica, sans-serif;}h1{text-align: center; flex: 0 1 auto;}#gui{flex: 1 1 auto; display: flex; flex-flow: column; flex-wrap wrap;}#guitop{display: flex; flex-flow: row; justify-content: space-between; flex-wrap wrap;}#logtext{flex: 1 1 auto;}#constatus.good{color: green;}#constatus.bad{s color: red;}</style></head><body> <h1>Node 6EF678 Status</h1> <div id=\"gui\"> <div id=\"guitop\"> <span>Log Connection: <span id=\"constatus\" class=\"bad\">Initializing...</span></span> <label id=\"checklabel\"> <input type=\"checkbox\" id=autoscroll checked=true> Autoscroll</label> </div><textarea readonly=\"true\" id=\"logtext\"></textarea> </div></body><script>function setConStatus(status, text){showconnected=status; var statel=document.getElementById(\"constatus\"); statel.innerText=text; statel.className=showconnected ? \"good\" : \"bad\";}; function addMsg(text){var currentDate=new Date().toLocaleString(); var area=document.getElementById(\"logtext\"); var scroll=document.getElementById(\"autoscroll\").checked; area.value +=currentDate + \": \" + text + \"\\n\"; if (scroll) area.scrollTop=9999999;}; var connection=null; var loc=window.location,conuri; if (loc.protocol===\"https:\"){conuri=\"wss:\";}else{conuri=\"ws:\";}conuri +=\"//\" + loc.host; conuri +=\"/log\"; window.onload=function(){function connect(){addMsg('Connecting...'); setConStatus(false, 'Connecting'); connection=new WebSocket(conuri); connection.onopen=function(){addMsg('Connection open!'); setConStatus(true, 'Connected');}; connection.onmessage=function(e){addMsg(e.data);}; connection.onerror=function(e){addMsg(\"Connection Error\");}; connection.onclose=function(){setConStatus(false, 'Disconnected'); addMsg('Connection closed, retrying...');};}; function refreshConnectionHealth(){if (!connection || connection.readyState==3) connect();};function wsheartbeat(){if(connection.readyState===connection.OPEN){connection.send(\"heartbeat\");}}var heartbeat=setInterval(wsheartbeat,1000); connect(); setInterval(refreshConnectionHealth, 1000);};</script></html>";


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void scan(AsyncWebServerRequest *request) {
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
}


void StatusServer::begin(){
     server.addHandler(&ws);
      server.on("/hi", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html);
  });
  server.on("/scan", HTTP_GET, scan);
  server.onNotFound(notFound);
  server.begin();
  ready=true;
}

void StatusServer::log(const char* new_action){
  if(ready)
   ws.printfAll(new_action);
 }
