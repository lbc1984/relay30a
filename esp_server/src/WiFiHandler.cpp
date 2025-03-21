#include "WiFiHandler.h"
#include <WiFiManager.h>

String mac_address = "";
String deviceName = "";
WiFiClientSecure clientSecure;
WiFiClientSecure mqttSecure;
HTTPClient httpClient;
WiFiManager wifiManager;

void setup_wifi()
{
  clientSecure.setInsecure();
  mqttSecure.setInsecure();

  mac_address = WiFi.macAddress();

  wifiManager.setConfigPortalTimeout(60);
  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}
