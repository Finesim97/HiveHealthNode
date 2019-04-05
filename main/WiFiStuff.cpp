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

boolean WifiCon::connect(boolean ap_enabled){
  const char *ssid=ssidbuffer, *id=identitybuffer, *wifipass=wifipassbuffer, *appass=appassbuffer;
  if(ap_enabled&&WiFi.getMode()!=WIFI_AP_STA){
    strcpy(appassbuffer,WIFI_APPASS_DEF);
    pref.getString(WIFI_APPASS,appassbuffer, WIFI_APPASS_MAXLENGTH+1);
    WiFi.mode(WIFI_AP_STA);
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
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)identitybuffer, strlen(identitybuffer));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)identitybuffer, strlen(identitybuffer)); 
    strcpy(userpassbuffer,WIFI_USERPASS_DEF);
    pref.getString(WIFI_USERPASS,userpassbuffer, WIFI_USERPASS_MAXLENGTH+1);
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)userpassbuffer, strlen(userpassbuffer)); 
    WiFi.begin(ssid); //connect to wifi
  }else if(pref.getBool(WIFI_USEWPS,WIFI_USEWPS_DEF)){
    esp_wifi_wps_disable();
    esp_wps_config_t c =wpsInitConfig(); 
    esp_wifi_wps_enable(&c);
    WiFi.begin();
  }else{
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
