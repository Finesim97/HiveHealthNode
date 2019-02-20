#include "MQTTService.h" 

// See the header for documentation

MQTTService::MQTTService(Preferences &_pref, Client &_wifi, SensorReading* _sensors, int _noofsensors):pref(_pref){
  sensors=_sensors;
  noofsensors=_noofsensors;
  pref.getString(MQTT_HOSTNAME,hostnamebuffer, MQTT_HOSTNAME_MAXLENGTH+1);
  mqtt = PubSubClient((const char*)hostnamebuffer, pref.getUShort(MQTT_PORT,MQTT_PORT_DEF), _wifi);
}

boolean MQTTService::loop(){
  return mqtt.loop(); 
}

void MQTTService::begin(){

}

boolean MQTTService::connect(){
  clientnamebuffer[0] = (char)0;
  usernamenamebuffer[0] = (char)0;
  passwordbuffer[0] = (char)0;
  const char *id=clientnamebuffer, *user=usernamenamebuffer, *pass=passwordbuffer;
  pref.getString(MQTT_CLIENTNAME,clientnamebuffer, MQTT_CLIENTNAME_MAXLENGTH+1);
  pref.getString(MQTT_USERNAME,usernamenamebuffer, MQTT_USERNAME_MAXLENGTH+1);
  pref.getString(MQTT_PASS,passwordbuffer, MQTT_PASS_MAXLENGTH+1);
  if(strlen(user)==0){
    user=NULL;
  }
  if(strlen(pass)==0){
    pass=NULL;
  }
  mqtt.connect(id, user,pass);
}

void MQTTService::disconnect(){
  mqtt.disconnect();
}

char* constructPrefName(char* prefnamebuffer, SensorReading* sr, const char* suffix){
  strcpy(prefnamebuffer,sr->sensorbaseName);
  strcat(prefnamebuffer,suffix);
  return prefnamebuffer;
}

boolean MQTTService::publishSensorReading(SensorReading* sr){;
  topicbuffer[0]=(char)0;
  measurementbuffer[0]=(char)0;
  const char *topic=topicbuffer;
  char prefnamebuffer[64];
  pref.getString(constructPrefName(prefnamebuffer,sr, SUFFIX_TOPIC),topicbuffer, TOPICBUFFERLEN);
  if(strlen(topic)==0){
    topic=sr->sensorbaseName;
  }
  boolean retain = pref.getBool(constructPrefName(prefnamebuffer,sr, SUFFIX_RETAIN),sr->retained_def);
  boolean measurementr = sr->measure(measurementbuffer);
  if(measurementr) {
    Serial.println(topic);
   return mqtt.publish(topic,measurementbuffer,retain);
   }
   return false;
}

uint32_t MQTTService::getWaitTime(SensorReading *sr){
   char prefnamebuffer[64];
  return pref.getUInt(constructPrefName(prefnamebuffer,sr, SUFFIX_WAIT),sr->sleepsecs);
}

// Small helper to find the greatest common divisor
uint32_t gcd(uint32_t a, uint32_t b){
  while(b!=0){
    uint32_t h = a%b;
    a = b;
    b = h;
  }
  return a;
}

uint32_t MQTTService::getWaitTimeInterval(){   
  uint32_t res=getWaitTime(&sensors[0]);
  for(int i=1;i<noofsensors;i++){
    res=gcd(res,getWaitTime(&sensors[i]));
   }
   return res;
}

uint32_t MQTTService::getMaxWaitTime(){   
  uint32_t res=getWaitTime(&sensors[0]);
  for(int i=1;i<noofsensors;i++){
    res=std::max(res,getWaitTime(&sensors[i]));
   }
   return res;
}

bool MQTTService::deepSleepLoop(uint32_t &sleeped){
    boolean connected=connect();
    if(!connected){
      return false;
    }
    for(int i=0;i<noofsensors;i++){
     if((sleeped%getWaitTime(&sensors[i]))==0){
      publishSensorReading(&sensors[i]);
     }
   }
   disconnect();
   if(sleeped%getMaxWaitTime()==0){
    sleeped=0;
   }
}
