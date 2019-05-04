#include "SetupServer.h"

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

void SetupServer::log(const char* new_action){
  if(ready)
   ws.printfAll(new_action);
 }

JsonObject searchForObj(JsonArray &arr, const char* fieldname, const char* value){
  for(JsonObject obj:arr){
      JsonVariant currentval = obj[fieldname];
      if(!currentval.isNull()&&strcmp(value,currentval)==0){
        return obj;
       }
    }
    return JsonObject();
}


#define STRINGFIELDS 4
void addPreferenceString(JsonVariant ja, char* name,const uint8_t maxlen, char* defvalue, char* currentvalue){
  JsonObject obj1 = ja.createNestedObject();
  obj1["name"] = name;
  obj1["max"] = maxlen;
  obj1["def_value"] = defvalue;
  obj1["cur_value"] = currentvalue;
}

void addPreferenceString(JsonVariant ja, char* name,const uint8_t maxlen,char* defvalue,Preferences &p){
  char buffer[maxlen+1];
  strcpy(buffer,defvalue);
  p.getString(name,buffer, maxlen+1);
  addPreferenceString(ja,name,maxlen,defvalue,buffer);
}

boolean setPreferenceString(JsonObject &obj, Preferences &p,JsonArray &ref){
   JsonVariant namevalue = obj["name"];
   if(namevalue.isNull()){
    return false;
   }
  JsonObject refobj=searchForObj(ref,"name",namevalue);
  if(refobj.isNull()){
    return false;
    }
   uint8_t maxlen = refobj["max"];
   const char* currentvalue = refobj["cur_value"];
   JsonVariant new_val = obj["cur_value"];
   if(new_val.isNull()){
    return false;
   }
   const char* newstring= new_val;
   size_t len = strlen(newstring);
   if(len==0||len>maxlen){
    return false;
    }
    if(strcmp(newstring,currentvalue)==0){
      return true;
    }
    return p.putString(namevalue,newstring) >0;
}


#define NUMBERFIELDS 5
template< typename T > void addPreferenceNumber(JsonVariant ja, char* name,const T min,const T max,const  T defvalue,const T currentvalue){
  JsonObject obj1 = ja.createNestedObject();
  obj1["name"] = name;
  obj1["min"] = min;
  obj1["max"] = max;
  obj1["def_value"] = defvalue;
  obj1["cur_value"] = currentvalue;
}

void addPreferenceNumber(JsonVariant ja, char* name,const uint16_t min,const uint16_t max,const  uint16_t defvalue,Preferences &p){
  const uint16_t val = p.getUShort(name,defvalue);
   addPreferenceNumber(ja,name,min,max,defvalue,val);
}

boolean setPreferenceUShort(JsonObject &obj, Preferences &p,JsonArray &ref){
   JsonVariant namevalue = obj["name"];
   if(namevalue.isNull()){
    return false;
   }
  JsonObject refobj=searchForObj(ref,"name",namevalue);;
  if(refobj.isNull()){
    return false;
    }
   uint16_t max = refobj["max"];
   uint16_t min = refobj["min"];
   uint16_t currentvalue = refobj["cur_value"];
   JsonVariant new_val = obj["cur_value"];
   if(new_val.isNull()){
    return false;
   }
   uint16_t newnum= new_val;
   if(newnum>max||newnum<min){
    return false;
    }
    if(newnum==currentvalue){
      return true;
     }
    return p.putUShort(namevalue,newnum) >0;
}

#define BOOLEANFIELDS 3
void addPreferenceBool(JsonVariant ja, char* name,const bool defvalue,const bool currentvalue){
  JsonObject obj1 = ja.createNestedObject();
  obj1["name"] = name;
  obj1["def_value"] = defvalue;
  obj1["cur_value"] = currentvalue;
}

void addPreferenceBool(JsonVariant ja, char* name,const bool defvalue,Preferences &p){
  const bool val=p.getBool(name,defvalue);
   addPreferenceBool(ja,name,defvalue,val);
}

boolean setPreferenceBool(JsonObject &obj, Preferences &p,JsonArray &ref){
   JsonVariant namevalue = obj["name"];
   if(namevalue.isNull()){
    return false;
   }
  JsonObject refobj=searchForObj(ref,"name",namevalue);;
  if(refobj.isNull()){
    return false;
    }
   bool currentvalue = refobj["cur_value"];
   JsonVariant new_val = obj["cur_value"];
   if(new_val.isNull()){
    return false;
   }
   bool newnbool= new_val;
    if(newnbool==currentvalue){
      return true;
     }
    return p.putBool(namevalue,newnbool) >0;
}

#define sensorbooleanprefs 1
#define sensorstringprefs 1
#define sensornumberprefs 1
void addPreferenceSensor(JsonVariant strings,JsonVariant numbers, JsonVariant bools, SensorReading &sr, Preferences &p){
  char prefnamebuffer[PREFFBUFFERSIZE];
  addPreferenceBool(bools,constructPrefName(prefnamebuffer,sr,SUFFIX_RETAIN),sr.retained_def,p);
  char sensorname[SENSORNAMELEN+1];
  strcpy(sensorname,sr.sensorbaseName);
  addPreferenceString(strings,constructPrefName(prefnamebuffer,sr,SUFFIX_TOPIC),PREFFBUFFERSIZE-1,sensorname,p);
  char measurementbuffer[TOPICMEASUREBUFFERLEN];
  sr.measure(measurementbuffer);
  addPreferenceString(strings,constructPrefName(prefnamebuffer,sr,"-measured"),0,"-",measurementbuffer);
  addPreferenceNumber(numbers,constructPrefName(prefnamebuffer,sr,SUFFIX_WAIT),1,ULONG_MAX/1000/1000-1,sr.sleepsecs,p);
}

const int strobj=(3+5)+SENSORS*sensorstringprefs, nuobj=(1+0)+SENSORS*sensornumberprefs, boolobj=(0+2)+SENSORS*sensorbooleanprefs;
const int capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(SENSORS) + JSON_ARRAY_SIZE(strobj) + JSON_ARRAY_SIZE(nuobj) + strobj*JSON_OBJECT_SIZE(STRINGFIELDS) + 
    nuobj*JSON_OBJECT_SIZE(NUMBERFIELDS)+ JSON_ARRAY_SIZE(boolobj)+boolobj*JSON_OBJECT_SIZE(BOOLEANFIELDS) + MQTT_HOSTNAME_MAXLENGTH +MQTT_USERNAME_MAXLENGTH+MQTT_PASS_MAXLENGTH+
    WIFI_SSID_MAXLENGTH+WIFI_PASS_MAXLENGTH+WIFI_APPASS_MAXLENGTH+WIFI_IDENTITY_MAXLENGTH+WIFI_USERPASS_MAXLENGTH+3*2+SENSORS*(SENSORNAMELEN+1+PREFFBUFFERSIZE*2+64)+128;

void constructCurrentConfigJSON(JsonDocument &d, Preferences &p){
 JsonObject root = d.to<JsonObject>();
 JsonArray strings = root.createNestedArray(STRINGS);
 JsonArray numbers = root.createNestedArray(NUMBERS);
 JsonArray sensorsarr = root.createNestedArray(SENSORNAME);
 JsonArray bools = root.createNestedArray(BOOLS);
 
 addPreferenceString(strings,MQTT_HOSTNAME,MQTT_HOSTNAME_MAXLENGTH,MQTT_HOSTNAME_DEF,p);
 addPreferenceNumber(numbers,MQTT_PORT,MQTT_PORT_MIN,MQTT_PORT_MAX,MQTT_PORT_DEF,p);
 addPreferenceString(strings,MQTT_USERNAME,MQTT_USERNAME_MAXLENGTH,MQTT_USERNAME_DEF,p);
 addPreferenceString(strings,MQTT_PASS,MQTT_PASS_MAXLENGTH,MQTT_PASS_DEF,p);


 addPreferenceString(strings,WIFI_SSID,WIFI_SSID_MAXLENGTH,WIFI_SSID_DEF,p);
 addPreferenceString(strings,WIFI_PASS,WIFI_PASS_MAXLENGTH,WIFI_PASS_DEF,p);
 addPreferenceString(strings,WIFI_APPASS,WIFI_APPASS_MAXLENGTH,WIFI_APPASS_DEF,p);
 addPreferenceString(strings,WIFI_IDENTITY,WIFI_IDENTITY_MAXLENGTH,WIFI_IDENTITY_DEF,p);
 addPreferenceString(strings,WIFI_USERPASS,WIFI_USERPASS_MAXLENGTH,WIFI_USERPASS_DEF,p);
 addPreferenceBool(bools,WIFI_USEEAP,WIFI_USEEAP_DEF,p);
 addPreferenceBool(bools,WIFI_USEWPS,WIFI_USEWPS_DEF,p);

 
 for(int i=0;i<SENSORS;i++){
  SensorReading sr = sensors[i];
    char sensorname[SENSORNAMELEN+1];
  strcpy(sensorname,sr.sensorbaseName);
  sensorsarr.add(sensorname);
  addPreferenceSensor(strings,numbers,bools,sr,p);
 }
}

void printCurrentConfigJSON(Print &printer){
 StaticJsonDocument<capacity> d;
 constructCurrentConfigJSON(d,preferences);
 serializeJson(d, printer);
}

bool setConfigJSON(char* jsonstr){
 Preferences &p=preferences;
 StaticJsonDocument<capacity> input;
 DeserializationError inputerr = deserializeJson(input, jsonstr);
 if (inputerr) {
   Serial.print(F("deserializeJson() returned ")); Serial.println(inputerr.c_str());
   return false;
  }
  JsonObject root = input.as<JsonObject>();
  DynamicJsonDocument reference(capacity);
  constructCurrentConfigJSON(reference,p);
  for(JsonObject string: root[STRINGS].as<JsonArray>()) {
    JsonArray ref = reference[STRINGS].as<JsonArray>();
    boolean success = setPreferenceString(string, p,ref);
    if(!success){
      return false;
      }
  }
  for(JsonObject number:root[NUMBERS].as<JsonArray>()) {
      JsonArray ref = reference[NUMBERS].as<JsonArray>();
      boolean success = setPreferenceUShort(number, p,ref);
      if(!success){
        return false;
      }
  }
  for(JsonObject boolel:root[BOOLS].as<JsonArray>()) {
      JsonArray ref = reference[BOOLS].as<JsonArray>();
      boolean success = setPreferenceBool(boolel, p,ref);
      if(!success){
         return false;
      }
  }
  return true;
}

void getCurrentConfig(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  printCurrentConfigJSON(*response);
  request->send(response);
}

void setCurrentConfig(AsyncWebServerRequest *request) {
      if (request->_tempObject != NULL) {
        boolean worked=setConfigJSON((char*)(request->_tempObject));
        if(worked){
           request->send(204,"text/plain", "Wrote updated entries!\n");
          }else{
           request->send(500,"text/plain", "Error while updating entries!\n");
          }
        }
      request->send(400,"text/plain","Missing data!\n");
}

void handleJSONBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (total > 0 && request->_tempObject == NULL && total < capacity) {
        request->_tempObject = malloc(total+1);
        char* objptr=(char*)(request->_tempObject);
        objptr[total]='\0';
      }
      if (request->_tempObject != NULL) {
        memcpy((uint8_t*)(request->_tempObject) + index, data, len);
      }
}
void handleJSONUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  //TODO
}

void SetupServer::begin(){
     server.addHandler(&ws);
      server.on("/hi", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/html", index_html);
  });
  server.on("/scan", HTTP_GET, scan);
  server.on("/config", HTTP_GET, getCurrentConfig);
  server.on("/config", HTTP_POST, setCurrentConfig,handleJSONUpload,handleJSONBody);
  server.on("/restart", HTTP_POST, [](AsyncWebServerRequest *request){
  request->send(200, "text/html", "Restarting...");
    delay(500);
    ESP.restart();
  });
  server.onNotFound(notFound);
  server.begin();
  ready=true;
}
