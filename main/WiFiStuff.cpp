#include "WiFiStuff.h"

esp_wps_config_t WifiCon::wpsInitConfig(){
  esp_wps_config_t config;
  config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, apname);
  return config;
}

WifiCon::WifiCon(Preferences &_pref, char* _apname):pref(_pref){
  apname=_apname;
}

void WifiCon::startWPS(){
    esp_wifi_wps_disable();
    esp_wps_config_t c =wpsInitConfig(); 
    esp_wifi_wps_enable(&c);
    esp_wifi_wps_start(0);
}


IPAddress apIP(192, 168, 1, 1);
boolean WifiCon::connect(boolean ap_enabled){
  const char *ssid=ssidbuffer, *id=identitybuffer, *wifipass=wifipassbuffer, *appass=appassbuffer, *upass=userpassbuffer;
  if(ap_enabled&&WiFi.getMode()!=WIFI_AP_STA){
    strcpy(appassbuffer,WIFI_APPASS_DEF);
    pref.getString(WIFI_APPASS,appassbuffer, WIFI_APPASS_MAXLENGTH+1);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if(strlen(appassbuffer)>0){
       WiFi.softAP(apname,appass);
    }else{
       WiFi.softAP(apname);
    }
  }
  if(!ap_enabled&&WiFi.getMode()!=WIFI_STA){
     WiFi.mode(WIFI_STA);
  }
  strcpy(ssidbuffer,WIFI_SSID_DEF);
  pref.getString(WIFI_SSID,ssidbuffer, WIFI_SSID_MAXLENGTH+1);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < WIFITRIES) {
  if(pref.getBool(WIFI_USEEAP,WIFI_USEEAP_DEF)){
    strcpy(identitybuffer,WIFI_IDENTITY_DEF);
    pref.getString(WIFI_IDENTITY,identitybuffer, WIFI_IDENTITY_MAXLENGTH+1);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)id, strlen(id));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)id, strlen(id)); 
    strcpy(userpassbuffer,WIFI_USERPASS_DEF);
    pref.getString(WIFI_USERPASS,userpassbuffer, WIFI_USERPASS_MAXLENGTH+1);
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)upass, strlen(upass)); 
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
    esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
    WiFi.begin(ssid); //connect to wifi
  }else if(pref.getBool(WIFI_USEWPS,WIFI_USEWPS_DEF)){
    esp_wifi_sta_wpa2_ent_disable();
    esp_wifi_wps_disable();
    esp_wps_config_t c =wpsInitConfig(); 
    esp_wifi_wps_enable(&c);
    WiFi.begin();
  }else{
    esp_wifi_sta_wpa2_ent_disable();
     strcpy(wifipassbuffer,WIFI_PASS_DEF);
     pref.getString(WIFI_PASS,wifipassbuffer, WIFI_PASS_MAXLENGTH+1);
     if(strlen(wifipassbuffer)>0){
           WiFi.begin(ssid,wifipass);
     }else{
           WiFi.begin(ssid);
     }
  }
    delay(WIFIWAIT_MS);
    tries++;
  }
  return WiFi.status() == WL_CONNECTED;
 }


 void WiFiEvent(WiFiEvent_t event)
{
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
    }}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
};
