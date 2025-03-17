#include "WiFiHandler.h"

String mac_address = "";
String deviceName = "";
WiFiClientSecure clientSecure;
HTTPClient httpClient;
WiFiManager wifiManager;

void setup_wifi()
{
  clientSecure.setInsecure();
  mac_address = WiFi.macAddress();

  wifiManager.setConfigPortalTimeout(60);
  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}
