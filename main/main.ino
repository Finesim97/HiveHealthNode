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
#define LED_PIN 2 // Which pin controls the LED

#define LOOPTIME 1 // How many seconds should be spent in the loop() method
#define WIFITRIES 10// How many times should we check again for ip connectivity
#define WIFIWAIT_MS 1000 // How long to wait each try for WiFi to come available
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
uint16_t secswifilost = 0; // How many seconds did it take for wifi to come online, subtract that from the next sleep time
unsigned long loopstarted = 0; // At which time (millis()) did we enter the loop method for the first time
boolean setupneeded=true;


/*
 * Start every library and connect to the network
 */
void setup() {
  if(setupneeded){
  Serial.begin(BAUDRATE);
  setState("Setup");
  preferences.begin(PREFERENCE_NAMESPACE,false);
  LEDbegin(LED_PIN);
  setStatus(led_booting);
  WiFi.onEvent(WiFiEvent);
  setState("WiFi Start");
  char* clientname=mqtt.getClientName();
  boolean ap_en=true;
  if(ap_en){
  WiFi.mode(WIFI_AP_STA);
  setState("Starting Wifi AP");
  WiFi.softAP(clientname);
  }else{
     WiFi.mode(WIFI_STA);
  }
  //WiFi.begin();
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFITRIES) {
    setState("Waiting for connection...");
    delay(WIFIWAIT_MS);
    tries++;
  }
  if(WiFi.status() != WL_CONNECTED ){
    setState("Unable to connect!");
  }else{
    setState("Connected!");
  }
    setState("Starting mDNS responder");
  boolean mdnsstart=MDNS.begin(clientname);
  if(mdnsstart){
      MDNS.addService("http", "tcp", 80);
    }else{
        setState("Error starting mDNS!");
   }
    statusserver.begin();
  secswifilost = tries;
  setupneeded=false;
  }
  setState("Starting MQTT connection and sensor reading");
  mqtt.deepSleepLoop(secsleeped);
  setState("Going into loop() calls");
  loopstarted = millis();
  firstboot = false;
}

/*
 * Go into to deep sleep for "secs" second, while disconnecting every connection and considering
 * wifi connect time and the time spent in the loop() method calls.
 */
void dsleep(uint32_t secs) {
  boolean nodsleep=preferences.getBool(NODSLEEP_PREF,true);
  if(nodsleep){
    setState("Preparing for normal sleep...");
  }else{
        setState("Preparing for deep sleep...");
  }
  setStatus(led_off);
  uint32_t actualsecs = secs - LOOPTIME - secswifilost;
  secsleeped += secs;
  if(nodsleep){
    delay(actualsecs*1000);
    setup();
    }else{
  esp_sleep_enable_timer_wakeup(actualsecs * 1000000);
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
