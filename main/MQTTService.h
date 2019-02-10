#if !defined _MQTTSERVICE
#define _MQTTSERVICE

#include <PubSubClient.h>
#include <WiFi.h>
#include <Preferences.h>


#define MQTT_HOSTNAME  "mqtthostname"
#define MQTT_HOSTNAME_MAXLENGTH 255  // Not including NULL character!
#define MQTT_HOSTNAME_DEF  "hostname"

#define MQTT_PORT "mqttport"
#define MQTT_PORT_DEF 1883  // Datatype: uint16_t

#define MQTT_CLIENTNAME "clientname"
#define MQTT_CLIENTNAME_MAXLENGTH 127
#define MQTT_CLIENTNAME_DEF "hivehealthnode"

#define MQTT_USERNAME "mqttusername"
#define MQTT_USERNAME_MAXLENGTH 127
#define MQTT_USERNAME_DEF "" //No username

#define MQTT_PASS "mqttusername"
#define MQTT_PASS_MAXLENGTH 127
#define MQTT_PASS_DEF "" //No password

#define TOPICBUFFERLEN 128

#define SUFFIX_RETAIN "-retain"
#define SUFFIX_TOPIC "-topic"
#define SUFFIX_WAIT "-sleep"


struct SensorReading{
  const char sensorbaseName[64];
  const boolean retained_def;
  boolean (*measure)(char* strbuffer);
  const uint8_t sleepsecs;
};

const char* constructPrefName(const char* prefnamebuffer, SensorReading* sr, const char* suffix);

class MQTTService{
    public:
      void begin();
      boolean loop();
      boolean connect();
      boolean publishSensorReading(SensorReading *sr);
      uint8_t getWaitTime(SensorReading *sr);
      void disconnect();
      MQTTService(Preferences &_pref, Client &_wifi);
    private:
      void callback(char* topic, byte* payload, unsigned int length);
      Preferences &pref;
      PubSubClient mqtt;
      char hostnamebuffer[MQTT_HOSTNAME_MAXLENGTH+1] = MQTT_HOSTNAME_DEF;
      char clientnamebuffer[MQTT_CLIENTNAME_MAXLENGTH+1]=MQTT_CLIENTNAME_DEF;
      char usernamenamebuffer[MQTT_USERNAME_MAXLENGTH+1]=MQTT_USERNAME_DEF;
      char passwordbuffer[MQTT_PASS_MAXLENGTH+1]=MQTT_PASS_DEF;
      char measurementbuffer[MQTT_MAX_PACKET_SIZE];
      char topicbuffer[TOPICBUFFERLEN];
};
#endif
