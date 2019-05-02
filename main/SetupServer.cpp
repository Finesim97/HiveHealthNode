#include "SetupServer.h"


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

void getCurrentConfigJSON(Preferences &p){
 StaticJsonDocument<capacity> d;
 constructCurrentConfigJSON(d,p);
 serializeJsonPretty(d, Serial);
}

bool setConfigJSON(Preferences &p, char* jsonstr){
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
