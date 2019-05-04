/*
  For ESP32 board support follow this document:
  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <WiFi.h> // WiFi Connectivity
#include <Preferences.h> // Storing configuration in non volatile storage
#include <ESPmDNS.h> //blabla.local hostnames
#include "esp_system.h"

#include "Sensors.h"
#include "MQTTService.h" // MQTT and Sensor connectivity
#include "SetupServer.h" // HTTP Setup Server, only run at specific times
#include "StatusLED.h" // Showing current activity with the LED
#include "WiFiStuff.h" //WiFi Helper methods
#include "SetupServer.h"


#define PREFERENCE_NAMESPACE "hhnode" //Needed for the preference lib
#define NODSLEEP_PREF "nodsleep" //Preference to prevent deepsleep
#define NODSLEEP_PREF_DEF true //Preference to prevent deepsleep default val
#define LED_PIN 2 // Which pin controls the LED
#define SETUPRUN "setuprun" //Preference to store, whether the user setup has been run before

#define LOOPTIME 1 // How many seconds should be spent in the loop() method
#define MINDSLEEPTIME 30 //At which sleep intervall (sec) should we go into deep sleep, gets overwritten by the NODSLEEP_PREF
#define BAUDRATE 115200 // Serial Baudrate
#define WIFITESTINTERVAL 20 // How many seconds should be between a wifi setup try (secs)

WiFiClient wifi; // Object to interact with the Wifi class
Preferences preferences; // Helper to store the user settings in NVS
SetupServer setupserver(80); //Setup Server
/*
 * Set the current action and writing it to the serial connection and the webserver
 */
void setState(const char* new_action){
  Serial.println(new_action);
  setupserver.log(new_action);
}
MQTTService mqtt (preferences, wifi, sensors, SENSORS, setState); // Connects to the mqtt service and reads the sensors
char* constructName(){
 uint8_t mac[6]; //3 4 5 device
 esp_read_mac(mac, ESP_MAC_WIFI_STA);
 char str[14] = {0};
 sprintf(str,"hhnode-%02X%02X%02X", mac[3], mac[4], mac[5]);
 return str;
}
char* devname = constructName();
WifiCon wificon (preferences,devname);
void (*wifiEventLogger) (const char*) = setState;

RTC_DATA_ATTR boolean firstboot = true; // Is this the first time booting after a reset or just after a deep sleep
RTC_DATA_ATTR uint32_t secsleeped = 0; // How many seconds has the node slept, gets reset when the max wait time for a sensor has been reached
uint16_t secssetuplost = 0; // How many seconds did it take for wifi to come online, subtract that from the next sleep time
unsigned long loopstarted = 0; // At which time (millis()) did we enter the loop method for the first time
unsigned long lastwificheck = 0; // At which time (millis()) did we check the wifi connection?
boolean setupneeded=true; // After only a "delay" sleep, we don't need to reinit some objects and APIs
boolean activatesetup=false; // To we need to start the setup server, also disables deep sleep 


/*
 * Start every library and connect to the network
 */
void setup() {
  long setupstart=millis();
  if(setupneeded){
    activatesetup=goIntoSetupMode();
    LEDbegin(LED_PIN);
    setStatus(led_booting);
    Serial.begin(BAUDRATE);
    setState("Setup");
    WiFi.onEvent(WiFiEvent);
    preferences.begin(PREFERENCE_NAMESPACE,false);

  }
  boolean connected=WiFi.status() == WL_CONNECTED;
  if(!connected&&lastwificheck==0||(millis()-lastwificheck)>WIFITESTINTERVAL*1000){
      lastwificheck = millis();
      setState("Trying WiFi Connection...");
      boolean connected=wificon.connect(activatesetup);
      setState(connected?"WiFi connected°!":"WiFi not connected!?");
  }
  boolean mdnsstart=MDNS.begin(devname);
  if(mdnsstart){
      MDNS.addService("http", "tcp", 80);
    }else{
      setState("Error starting mDNS!");
  }
  if(setupneeded){
    if(activatesetup)
      setupserver.begin();
    setupneeded=false;
  }
  // WiFi.printDiag(Serial);
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
  if(actualsecs <  MINDSLEEPTIME || nodsleep || activatesetup){
    delay(actualsecs*1000+1);
    setup();
    }else{
  esp_sleep_enable_timer_wakeup(actualsecs * 1000*1000);
  esp_deep_sleep_start();
    }
}

boolean goIntoSetupMode(){
  bool gogogo=!preferences.getBool(SETUPRUN,false);
  gogogo=true; //TODO Replace with button read for example
  return gogogo;
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
