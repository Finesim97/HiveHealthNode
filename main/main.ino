/*
  For ESP32 board support follow this document:
  https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/
#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <Preferences.h>

#include "MQTTService.h"
#include "SetupServer.h"
#include "StatusLED.h"
#include "StatusServer.h"
#include "WiFiStuff.h"
#include "Sensors.h"

#define PREFERENCE_NAMESPACE "hhnode"
#define LED_PIN 2
#define uS_TO_S_FACTOR 1000000

#define LOOPTIME 1
#define WIFIWAIT_MS 500


StatusLED led;
WiFiClient wifi;
Preferences preferences;
MQTTService mqtt (preferences, wifi, sensors, SENSORS);

RTC_DATA_ATTR boolean firstboot = true;
RTC_DATA_ATTR uint32_t secsleeped = 0;
uint16_t secswifilost = 0;
unsigned long loopstarted = 0;

void dsleep(uint32_t secs) {
  preferences.end();
  uint32_t actualsecs = secs - LOOPTIME - secswifilost;
  Serial.flush();
  secsleeped += secs;
  esp_sleep_enable_timer_wakeup(actualsecs * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void setup() {
  led.begin(LED_PIN);
  Serial.begin(115200);
  preferences.begin(PREFERENCE_NAMESPACE,false);
  //Serial.println(preferences.putUInt("timesensor-sl",60*5));
  WiFi.onEvent(WiFiEvent);
  WiFi.begin();
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 10) {
    delay(1000);
    tries++;
  }
  secswifilost = tries;
  mqtt.begin();
  mqtt.deepSleepLoop(secsleeped);
  loopstarted = millis();
  firstboot = false;
}

void loop() {
  led.loop();
  mqtt.loop();
  if (millis() - loopstarted >= LOOPTIME * 1000) {
    dsleep(mqtt.getWaitTimeInterval());
  }
  delay(20);
}
