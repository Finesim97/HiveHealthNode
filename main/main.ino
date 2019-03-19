/*
  For ESP32 board support follow this document:
  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <WiFi.h> // WiFi Connectivity
#include <Preferences.h> // Storing configuration in non volatile storage
#include <ESPmDNS.h> //blabla.local hostnames

#include "Sensors.h"
#include "MQTTService.h" // MQTT and Sensor connectivity
#include "SetupServer.h" // HTTP Setup Server, only run at specific times
#include "StatusLED.h" // Showing current activity with the LED
#include "StatusServer.h" // Showing current activity on a WebServer and Serial
#include "WiFiStuff.h" //WiFi Helper methods

#define PREFERENCE_NAMESPACE "hhnode" //Needed for the preference lib
#define NODSLEEP_PREF "nodsleep" //Preference to prevent deepsleep
#define NODSLEEP_PREF_DEF true //Preference to prevent deepsleep default val
#define LED_PIN 2 // Which pin controls the LED

#define LOOPTIME 1 // How many seconds should be spent in the loop() method
#define MINDSLEEPTIME 30 //At which sleep intervall (sec) should we go into deep sleep, gets overwritten by the NODSLEEP_PREF
#define BAUDRATE 115200 // Serial Baudrate

WiFiClient wifi; // Object to interact with the Wifi class
Preferences preferences; // Helper to store the user settings in NVS
StatusServer statusserver(80); //Status WS Server
/*
 * Set the current action and writing it to the serial connection and the webserver
 */
void setState(const char* new_action){
  Serial.println(new_action);
  statusserver.log(new_action);
}
MQTTService mqtt (preferences, wifi, sensors, SENSORS, setState); // Connects to the mqtt service and reads the sensors
void (*wifiEventLogger) (const char*) = setState;

RTC_DATA_ATTR boolean firstboot = true; // Is this the first time booting after a reset or just after a deep sleep
RTC_DATA_ATTR uint32_t secsleeped = 0; // How many seconds has the node slept, gets reset when the max wait time for a sensor has been reached
uint16_t secssetuplost = 0; // How many seconds did it take for wifi to come online, subtract that from the next sleep time
unsigned long loopstarted = 0; // At which time (millis()) did we enter the loop method for the first time
boolean setupneeded=true;


/*
 * Start every library and connect to the network
 */
void setup() {
    long setupstart=millis();
  if(setupneeded){
  Serial.begin(BAUDRATE);
  setState("Setup");
  preferences.begin(PREFERENCE_NAMESPACE,false);
  LEDbegin(LED_PIN);
  setStatus(led_booting);
  WiFi.onEvent(WiFiEvent);
  setupneeded=false;
  }
  setState("WiFi Start");
  char* clientname=mqtt.getClientName(); 
  boolean connected=connectWifi(preferences,true, clientname);
  statusserver.begin();
  setState(connected?"WiFi connected°!":"WiFi not connected!?");
    boolean mdnsstart=MDNS.begin(clientname);
  if(mdnsstart){
      MDNS.addService("http", "tcp", 80);
    }else{
        setState("Error starting mDNS!");
   }
  setState("Starting MQTT connection and sensor reading");
  mqtt.deepSleepLoop(secsleeped);
  setState("Going into loop() calls");
  secssetuplost = (millis()-setupstart)/1000;
  loopstarted = millis();
  firstboot = false;
}

/*
 * Go into to deep sleep for "secs" second, while disconnecting every connection and considering
 * wifi connect time and the time spent in the loop() method calls.
 */
void dsleep(uint32_t secs) {
    setState("Preparing for sleep.");
  boolean nodsleep=preferences.getBool(NODSLEEP_PREF,NODSLEEP_PREF_DEF);
  setStatus(led_off);
  int32_t secstosleep = secs - LOOPTIME - secssetuplost;
  uint32_t actualsecs = secstosleep>0?secstosleep:0;
  secsleeped += secs;
   setState("ZZZ... ZZZ...");
  if(actualsecs <  MINDSLEEPTIME || nodsleep){
    delay(actualsecs*1000+1);
    setup();
    }else{
  esp_sleep_enable_timer_wakeup(actualsecs * 1000*1000);
  esp_deep_sleep_start();
    }
}


/*
 * Some libraries may need time in the main loop to finish their operations
 */
void loop() {
  mqtt.loop();
  if (millis() - loopstarted >= LOOPTIME * 1000) {
    dsleep(mqtt.manageWaitTimeInterval(secsleeped));
  }
  delay(20);
}
