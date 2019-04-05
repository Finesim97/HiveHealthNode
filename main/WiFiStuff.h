#if !defined _WIFISTUFF
#define _WIFISTUFF
#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include "esp_wps.h" //WPS library
#include <Preferences.h> // Dealing with preferences


// SSID
#define WIFI_SSID  "ssid"
#define WIFI_SSID_MAXLENGTH 32  // Not including NULL character!
#define WIFI_SSID_DEF  ""
// Password
#define WIFI_PASS "wifipass"
#define WIFI_PASS_MAXLENGTH 127
#define WIFI_PASS_DEF ""
// Wifi ap password
#define WIFI_APPASS "wifiappass"
#define WIFI_APPASS_MAXLENGTH 127
#define WIFI_APPASS_DEF "" 
// Use Extensible Authentication Protocol 
#define WIFI_USEEAP "wifieap"
#define WIFI_USEEAP_DEF false
// Use WPS pushbutton data
#define WIFI_USEWPS "wifiwps"
#define WIFI_USEWPS_DEF false
#define ESP_WPS_MODE WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "HiveHealthNode"
// Username
#define WIFI_IDENTITY "wifiuser"
#define WIFI_IDENTITY_MAXLENGTH 127
#define WIFI_IDENTITY_DEF "" 
// Wifi user password
#define WIFI_USERPASS "wifiuserpass"
#define WIFI_USERPASS_MAXLENGTH 127
#define WIFI_USERPASS_DEF "" 


class WifiCon{
    public:
      WifiCon(Preferences &_pref, char* _apname); // Constructor that sets the variable
      boolean connect(boolean ap_enabled);
      void startWPS();
    private:
      Preferences &pref; // The started connection to NVS
      char* apname;
      esp_wps_config_t wpsInitConfig();
      //A bunch of reused buffers
      char ssidbuffer[WIFI_SSID_MAXLENGTH+1] = WIFI_SSID_DEF;
      char wifipassbuffer[WIFI_PASS_MAXLENGTH+1]=WIFI_PASS_DEF;
      char appassbuffer[WIFI_APPASS_MAXLENGTH+1]=WIFI_APPASS_DEF;
      char identitybuffer[WIFI_IDENTITY_MAXLENGTH+1]=WIFI_IDENTITY_DEF;
      char userpassbuffer[WIFI_USERPASS_MAXLENGTH+1]=WIFI_USERPASS_DEF;
};

#define WIFITRIES 4// How many times should we check again for ip connectivity
#define WIFIWAIT_MS 2500 // How long to wait each try for WiFi to come available

void WiFiEvent(WiFiEvent_t event);
extern void (*wifiEventLogger) (const char*);
boolean connectWifi(Preferences &pref, boolean ap_enabled, char* apname);
#endif
