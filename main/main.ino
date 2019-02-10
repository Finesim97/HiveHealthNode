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

#define PREFERENCE_NAMESPACE "HiveHealthNode"
#define LED_PIN 13
#define uS_TO_S_FACTOR 1000000

boolean measureMilis(char* strbuffer){
  sprintf(strbuffer, "%d", millis());
  return true;
}

SensorReading sensors[] {{"timesensor",true,measureMilis,10}};


StatusLED led;
WiFiClient wifi;
Preferences preferences;
MQTTService mqtt (preferences, wifi);
RTC_DATA_ATTR boolean firstboot = true;

void sleep(int secs){
  Serial.flush();
  esp_sleep_enable_timer_wakeup(secs * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void setup() {
  led.begin(LED_PIN);
  Serial.begin(9600);
  preferences.begin(PREFERENCE_NAMESPACE);
  WiFi.begin("ssid", "pass");
  mqtt.begin();
  firstboot=false;
}

void loop() {
  //TODO some loops return a boolean, maybe waiting for work to finsih?
  Serial.println("I am alive!");
  led.loop();
  mqtt.loop();
  delay(1000);
}
