#if !defined _STATUS_LED
#define _STATUS_LED
#include <Ticker.h>

#define LED_BOOTING_ON_TIME 500
#define LED_BOOTING_OFF_TIME 750

enum Status { led_off=0, led_booting, led_active, led_setup, led_panic };

void LEDbegin(int _pin);
void setStatus(Status st);
void LEDend();

#endif
