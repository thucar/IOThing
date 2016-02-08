#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "Config.h"


void configModeCallback ( void ) {
//  Serial.println("Entered config mode");
//  Serial.println(WiFi.softAPIP());
}

void resetWifiSettings( void ){
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.reset();
}

void initWiFi ( void ){
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect(ACCESS_POINT_NAME)) {
//    Serial.println("Failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    yield();
  }
  //if you get here you have connected to the WiFi
//  Serial.println("Connected...");
}

//Make sure we still have Internet connection
void checkWiFi ( void ){
  
}
