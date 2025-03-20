#include "WiFiHandler.h"
#include <WiFiManager.h>
#include "ca_cert.h"

String mac_address = "";
String deviceName = "";
WiFiClientSecure clientSecure;
WiFiClientSecure viberSecure;
WiFiClientSecure mqttSecure;
HTTPClient httpClient;
WiFiManager wifiManager;

void setup_wifi()
{
  clientSecure.setInsecure();
  viberSecure.setInsecure();
  // mqttSecure.setCACert(root_ca);
  mqttSecure.setInsecure();

  mac_address = WiFi.macAddress();

  wifiManager.setConfigPortalTimeout(60);
  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}
