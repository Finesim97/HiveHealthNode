#include "MQTTService.h" 
#include <numeric>

// See the header for documentation

MQTTService::MQTTService(Preferences &_pref, Client &_wifi, SensorReading* _sensors, int _noofsensors, void (*_logfunction) (const char*)):pref(_pref){
  sensors=_sensors;
  noofsensors=_noofsensors;
  logfunction=_logfunction;
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
  logfunction("Reading sensor settings");
  logfunction(sr->sensorbaseName);
  topicbuffer[0]=(char)0;
  measurementbuffer[0]=(char)0;
  const char *topic=topicbuffer;
  char prefnamebuffer[64];
  pref.getString(constructPrefName(prefnamebuffer,sr, SUFFIX_TOPIC),topicbuffer, TOPICBUFFERLEN);
  if(strlen(topic)==0){
    topic=sr->sensorbaseName;
  }
  boolean retain = pref.getBool(constructPrefName(prefnamebuffer,sr, SUFFIX_RETAIN),sr->retained_def);
  logfunction("Measuring...");
  boolean measurementr = sr->measure(measurementbuffer);
  if(measurementr) {
      logfunction("Publishing result");
       boolean published = mqtt.publish(topic,measurementbuffer,retain);
       if(!published){
          logfunction("Unable to publish!");
          logfunction("MQTT State:");
          logfunction(decodeMQTTStatus(mqtt.state()));
        }else{
          logfunction("Published!");
        }
       return published;
   }else{
      logfunction("Measurement failed!");
   }
   return false;
}

uint32_t MQTTService::getWaitTime(SensorReading *sr){
   char prefnamebuffer[64];
  return pref.getUInt(constructPrefName(prefnamebuffer,sr, SUFFIX_WAIT),sr->sleepsecs);
}

uint32_t gcd(uint32_t a, uint32_t b){
  while(b!=0){
    uint32_t h = a%b;
    a = b;
    b = h;
  }
  return a;
}

uint32_t lcm(uint32_t a, uint32_t b){
  uint32_t thegcd=gcd(a, b);
  return  thegcd ? (a / thegcd * b) : 0;
}

uint32_t MQTTService::manageWaitTimeInterval(uint32_t &sleeped){   
   uint32_t waittimes[noofsensors];
   for(int i=0;i<noofsensors;i++){
     waittimes[i]=getWaitTime(&sensors[i]);
   }
  uint32_t maxwait=std::accumulate(waittimes, &waittimes[noofsensors-1], 1, lcm);
  uint32_t waitinterval=std::accumulate(waittimes, &waittimes[noofsensors-1], waittimes[0],gcd);
  uint32_t nextsleeped;
  boolean breakl=false;
  for(nextsleeped=sleeped+waitinterval;nextsleeped<maxwait;nextsleeped+=waitinterval){
       for(int i=0;i<noofsensors;i++){
          if(nextsleeped%waittimes[i]==0){
            breakl=true;
            break;
            }
        }
        if(breakl){
          break;
         }
  }
  uint32_t tosleep=nextsleeped-sleeped;
  if(nextsleeped==maxwait){
    sleeped=0;
  }
   return tosleep;
}

const char* decodeMQTTStatus(int state){
        switch(state){
        case MQTT_CONNECTION_TIMEOUT: return ("MQTT timeout!");
        case MQTT_CONNECTION_LOST: return("MQTT connection lost!");
        case MQTT_CONNECT_FAILED: return("MQTT connection failed!");
        case MQTT_DISCONNECTED: return("MQTT disconnected!");
        case MQTT_CONNECTED: return("MQTT connected!");
        case MQTT_CONNECT_BAD_PROTOCOL: return("MQTT bad protocol answer!");
        case MQTT_CONNECT_BAD_CLIENT_ID: return("MQTT bad client id!");
        case MQTT_CONNECT_UNAVAILABLE: return("MQTT unavailable!");
        case MQTT_CONNECT_BAD_CREDENTIALS: return("MQTT bad credentials!");
        case MQTT_CONNECT_UNAUTHORIZED: return("MQTT unauthorized!");
      }
}

bool MQTTService::deepSleepLoop(uint32_t &sleeped){
    logfunction("Connecting to the MQTT Server...");
    boolean connected=connect();
    logfunction(decodeMQTTStatus(mqtt.state()));
    if(!connected){
      return false;
    }
    for(int i=0;i<noofsensors;i++){
     if((sleeped%getWaitTime(&sensors[i]))==0){
      publishSensorReading(&sensors[i]);
     }
   }
   logfunction("Disconnecting from the MQTT Server.");
   disconnect();
}
