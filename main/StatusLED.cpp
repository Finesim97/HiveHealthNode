#include "StatusLED.h"
#include "Arduino.h"

Ticker blinker1;
Ticker blinker2;
int LEDpin=2;
boolean active=false;

void LEDbegin(int _pin){
    active=true;
    LEDpin=_pin;
    pinMode(_pin, OUTPUT);
}

void setPin(int on) {
  digitalWrite(LEDpin,on);
}

void setStatus(Status st){
  if(!active){
    return;
  }
  blinker1.detach();
  blinker2.detach();
  setPin(0);
  if(st==led_booting){
    blinker1.attach_ms(LED_BOOTING_ON_TIME,setPin,1);
    blinker2.attach_ms(LED_BOOTING_ON_TIME+LED_BOOTING_OFF_TIME,setPin,0);
  }else if(led_active){
      setPin(1);
  }else if(led_panic){
  }else if(led_setup){
  }
}
