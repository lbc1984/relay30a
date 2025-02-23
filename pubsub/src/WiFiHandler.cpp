#include "WiFiHandler.h"

String mac_address = "";
String deviceName = "";

void setup_wifi()
{
  mac_address = WiFi.macAddress();
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(60);

  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}
