#if !defined _WIFISTUFF
#define _WIFISTUFF
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <Preferences.h> // Dealing with preferences

#define EAP_IDENTITY "XXXXXX" //if connecting from another corporation, use identity@organisation.domain in Eduroam 
#define EAP_PASSWORD "XXXXXX" //your Eduroam password

#define WIFITRIES 4// How many times should we check again for ip connectivity
#define WIFIWAIT_MS 2500 // How long to wait each try for WiFi to come available

void WiFiEvent(WiFiEvent_t event);
extern void (*wifiEventLogger) (const char*);
boolean connectWifi(Preferences &pref, boolean ap_enabled, char* apname);
#endif
