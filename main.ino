/*
For ESP32 board support follow this document:
https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <Arduino.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <MQTT.h>
/*
No idea how the partitioning works
Random sizes:
*/
EEPROMClass  NAMES("ssid", 0x1000);
EEPROMClass  NAMES("wlanpassword", 0x1000);

WiFiClient wifi;
MQTTClient mqtt;

void setup() {

  Serial.begin(9600);
  WiFi.begin("ssid", "pass");
  client.begin("brokerhostname",1000, wifi);
  client.connect("clientid","username","password");

}

void loop() {
  Serial.println("I am alive!");
  delay(1000);
 /* break it */
}
