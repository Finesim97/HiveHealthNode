#include "StatusLED.h"
#include "Arduino.h"

void StatusLED::begin(int _pin){
    pin=_pin;
    pinMode(pin, OUTPUT);
    Serial.print("Setup LED");
}
void StatusLED::loop(){
    Serial.print("Hi LED!");
}
