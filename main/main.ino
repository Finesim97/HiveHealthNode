/*
For ESP32 board support follow this document:
https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <MQTT.h>
#include "StatusLED.h"

#define LED_PIN 13


/*
No idea how the partitioning works
Random sizes:
*/
EEPROMClass  SSID("ssid", 0x1000);
EEPROMClass  PASS("wlanpassword", 0x1000);

StatusLED led;

WiFiClient wifi;
MQTTClient mqtt;


void setup() {
  led.begin(LED_PIN);
  Serial.begin(9600);
  WiFi.begin("ssid", "pass");
  mqtt.begin("brokerhostname",1000, wifi);
  mqtt.connect("clientid","username","password");

}

void loop() {
  Serial.println("I am alive!");
  led.loop();
  delay(1000);
 /* break it */
}
