#include "MQTTService.h" 

MQTTService::MQTTService(Preferences &_pref, Client &_wifi):pref(_pref){
  pref.getString(MQTT_HOSTNAME,hostnamebuffer, MQTT_HOSTNAME_MAXLENGTH+1);
  mqtt = PubSubClient((const char*)hostnamebuffer, pref.getUShort(MQTT_PORT,MQTT_PORT_DEF), _wifi);
}

boolean MQTTService::loop(){
  return mqtt.loop(); 
}

void MQTTService::begin(){
  //TODO nothing to do here
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

boolean MQTTService::publishSensorReading(SensorReading* sr){
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
   return mqtt.publish(topic,measurementbuffer);
   }
   return false;
}

uint8_t MQTTService::getWaitTime(SensorReading *sr){
   char prefnamebuffer[64];
  pref.getUShort(constructPrefName(prefnamebuffer,sr, SUFFIX_WAIT),sr->sleepsecs);
}
