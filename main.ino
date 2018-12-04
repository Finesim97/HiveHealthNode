/*
For ESP32 board support follow this document:
https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md
*/

#include <Arduino.h>

void setup() {
  Serial.begin(9600);

}

void loop() {
  Serial.println("I am alive!");
  delay(1000);
  break it
}
