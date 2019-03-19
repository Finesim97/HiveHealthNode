#include "WiFiStuff.h"

void WiFiEvent(WiFiEvent_t event)
{
  return;
    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            wifiEventLogger("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            wifiEventLogger("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            wifiEventLogger("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            wifiEventLogger("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            wifiEventLogger("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            wifiEventLogger("Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            wifiEventLogger("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            wifiEventLogger("Obtained IP address");
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            wifiEventLogger("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            wifiEventLogger("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            wifiEventLogger("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            wifiEventLogger("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            wifiEventLogger("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            wifiEventLogger("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            wifiEventLogger("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            wifiEventLogger("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            wifiEventLogger("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            wifiEventLogger("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            wifiEventLogger("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            wifiEventLogger("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            wifiEventLogger("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            wifiEventLogger("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            wifiEventLogger("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            wifiEventLogger("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            wifiEventLogger("Obtained IP address");
            break;
    }}

const char* ssid = "eduroam"; // Eduroam 

boolean connectWifi(Preferences &pref, boolean ap_enabled, char* apname){
  if(ap_enabled&&WiFi.getMode()!=WIFI_AP_STA){
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apname);
  }
  if(!ap_enabled&&WiFi.getMode()!=WIFI_STA){
     WiFi.mode(WIFI_STA);
  }
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFITRIES) {
   //WiFi.disconnect();
  if(true){
 esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); //provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
    WiFi.begin(ssid); //connect to wifi
  }else{
    WiFi.begin();
  }
    delay(WIFIWAIT_MS);
    tries++;
  }
  return WiFi.status() == WL_CONNECTED;
 }
