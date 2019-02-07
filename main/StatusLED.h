#if !defined _STATUS_LED
#define _STATUS_LED
enum Color { led_off, led_booting, led_setup, led_active, led_panic };
  class StatusLED{
    public:
      void begin(int _pin);
      void loop();
    private:
      int pin;
  };
#endif
