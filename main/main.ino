/*
  For ESP32 board support follow this document:
  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <WiFi.h> // WiFi Connectivity
#include <Preferences.h> // Storing configuration in non volatile storage

#include "MQTTService.h" // MQTT and Sensor connectivity
#include "SetupServer.h" // HTTP Setup Server, only run at specific times
#include "StatusLED.h" // Showing current activity with the LED
#include "StatusServer.h" // Showing current activity on a WebServer and Serial
#include "WiFiStuff.h" //WiFi Helper methods

#define PREFERENCE_NAMESPACE "hhnode" //Needed for the preference lib
#define LED_PIN 2 // Which pin controls the LED

#define LOOPTIME 1 // How many seconds should be spent in the loop() method
#define WIFITRIES 10// How many times should we check again for ip connectivity
#define WIFIWAIT_MS 1000 // How long to wait each try for WiFi to come available
#define BAUDRATE 115200 // 

WiFiClient wifi; // Object to interact with the Wifi class
Preferences preferences; // Helper to store the user settings in NVS
MQTTService mqtt (preferences, wifi, sensors, SENSORS); // Connects to the mqtt service and reads the sensors

RTC_DATA_ATTR boolean firstboot = true; // Is this the first time booting after a reset or just after a deep sleep
RTC_DATA_ATTR uint32_t secsleeped = 0; // How many seconds has the node slept, gets reset when the max wait time for a sensor has been reached
uint16_t secswifilost = 0; // How many seconds did it take for wifi to come online, subtract that from the next sleep time
unsigned long loopstarted = 0; // At which time (millis()) did we enter the loop method for the first time


/*
 * Go into to deep sleep for "secs" second, while disconnecting every connection and considering
 * wifi connect time and the time spent in the loop() method calls.
 */
void dsleep(uint32_t secs) {
  preferences.end();
  setStatus(led_off);
  uint32_t actualsecs = secs - LOOPTIME - secswifilost;
  Serial.flush();
  secsleeped += secs;
  esp_sleep_enable_timer_wakeup(actualsecs * 1000000);
  esp_deep_sleep_start();
}

/*
 * Start every library and connect to the network
 */
void setup() {
  preferences.begin(PREFERENCE_NAMESPACE,false);
  LEDbegin(LED_PIN);
  setStatus(led_booting);
  Serial.begin(BAUDRATE);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin();
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFITRIES) {
    delay(WIFIWAIT_MS);
    tries++;
  }
  secswifilost = tries;
  mqtt.begin();
  setStatus(led_active);
  mqtt.deepSleepLoop(secsleeped);
  loopstarted = millis();
  firstboot = false;
}

/*
 * Some libraries may need time in the main loop to finish their operations
 */
void loop() {
  mqtt.loop();
  if (millis() - loopstarted >= LOOPTIME * 1000) {
    dsleep(mqtt.getWaitTimeInterval());
  }
  delay(20);
}
