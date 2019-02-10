#include "StatusLED.h"
#include "Arduino.h"

void StatusLED::begin(int _pin){
    pin=_pin;
    pinMode(pin, OUTPUT);
}

void StatusLED::loop(){
}
