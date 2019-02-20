#if !defined _WIFISTUFF
#define _WIFISTUFF
#include <WiFi.h>
void WiFiEvent(WiFiEvent_t event);
extern void (*wifiEventLogger) (const char*);
#endif
