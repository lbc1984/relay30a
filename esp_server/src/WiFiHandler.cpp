#include "WiFiHandler.h"
#include <ESP_WiFiManager.h>

String mac_address = "", deviceName = "";
WiFiClientSecure clientSecure, viberSecure;
HTTPClient httpClient;
ESP_WiFiManager wifiManager;

void setup_wifi()
{
  clientSecure.setInsecure();
  viberSecure.setInsecure();
  mac_address = WiFi.macAddress();

  wifiManager.setConfigPortalTimeout(60);
  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}
