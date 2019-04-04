#if !defined _MQTTSERVICE
#define _MQTTSERVICE

#include <PubSubClient.h> // The used MQTT lib
#include <WiFi.h> // For the network client
#include <Preferences.h> // Dealing with preference

/*
 * Config names, default values and limitations
 */
// MQTT hostname
#define MQTT_HOSTNAME  "mqtthost"
#define MQTT_HOSTNAME_MAXLENGTH 255  // Not including NULL character!
#define MQTT_HOSTNAME_DEF  "broker.mqttdashboard.com"
// MQTT port
#define MQTT_PORT "mqttport"
#define MQTT_PORT_DEF 1883  // Datatype: uint16_t
#define MQTT_PORT_MAX 65535
#define MQTT_PORT_MIN 65535
// MQTT client name
#define MQTT_CLIENTNAME "cname"
#define MQTT_CLIENTNAME_MAXLENGTH 127
#define MQTT_CLIENTNAME_DEF "hivehealthnode"
// MQTT username
#define MQTT_USERNAME "mqttuser"
#define MQTT_USERNAME_MAXLENGTH 127
#define MQTT_USERNAME_DEF "" //No username
// MQTT password
#define MQTT_PASS "mqttpass"
#define MQTT_PASS_MAXLENGTH 127
#define MQTT_PASS_DEF "" //No password


// buffer size for the topics
#define TOPICMEASUREBUFFERLEN 127
#define PREFFBUFFERSIZE 63

// Suffixes for the different sensor related settings, used to generate the names
#define SENSORNAMELEN 31
#define SUFFIX_RETAIN "-rtn" // Suffix for the MQTT retain value (true/flase)
#define SUFFIX_TOPIC "-tc" // Suffix for the MQTT topic name
#define SUFFIX_WAIT "-sl"// Suffix for the sensor sleep time


/*
 * This struct describes a sensor
 */
struct SensorReading{ 
  const char sensorbaseName[SENSORNAMELEN]; // Basename of the sensor
  const boolean retained_def; // Should the value be retained in MQTT by default
  boolean (*measure)(char* strbuffer); // The function which measures this value
  const uint32_t sleepsecs; // What is the default measuring interval
};

/*
 * Construct the name of the setting in the given char buffer
 */
char* constructPrefName(char* prefnamebuffer, SensorReading& sr, const char* suffix); 

/*
 * Return a string describing the given mqtt.state() in a readable format
 */
const char* decodeMQTTStatus(int state);

/*
 * The class that describes the interaction with the MQTT service and the sensors
 */
class MQTTService{
    public:
      boolean loop(); // Just redirect that to the lib
      boolean connect(); // Start the connection, has it worked
      boolean publishSensorReading(SensorReading &sr); // Read and publish the given sensor
      uint32_t getWaitTime(SensorReading &sr); // What is the measurement interval for the given sensor
      uint32_t manageWaitTimeInterval(uint32_t &sleeped); // How long can we sleep, without missing a measurement, resets the sleeped when needed
      bool deepSleepLoop(uint32_t &sleeped); // Measure the sensors, that need to be measured now and publish the results
      void disconnect(); // Disconnect the sensors
      MQTTService(Preferences &_pref, Client &_wifi, SensorReading *_sensors, int _noofsensors, void (*logfunction) (const char*)); // Constructor that sets the variable
      char* getClientName();
    private:
      void (*logfunction) (const char*); // Action logger
      int noofsensors; // How long is the sensors array
      SensorReading *sensors; // The sensor array
      Preferences &pref; // The started connection to NVS
      PubSubClient mqtt; // The library client
      //A bunch of reused buffers
      char hostnamebuffer[MQTT_HOSTNAME_MAXLENGTH+1] = MQTT_HOSTNAME_DEF;
      char clientnamebuffer[MQTT_CLIENTNAME_MAXLENGTH+1]=MQTT_CLIENTNAME_DEF;
      char usernamenamebuffer[MQTT_USERNAME_MAXLENGTH+1]=MQTT_USERNAME_DEF;
      char passwordbuffer[MQTT_PASS_MAXLENGTH+1]=MQTT_PASS_DEF;
      char measurementbuffer[TOPICMEASUREBUFFERLEN];
      char topicbuffer[TOPICMEASUREBUFFERLEN];
};
#endif
