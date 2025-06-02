#include "WiFiHandler.h"

String mac_address = "";
String deviceName = "";
WiFiClientSecure clientSecure;
HTTPClient httpClient;
WiFiManager wifiManager;
deviceInfoStruct deviceInfo;

void setup_wifi()
{
  EEPROM.begin(512);
  deviceName = getDeviceName();

  clientSecure.setInsecure();
  mac_address = WiFi.macAddress();

  wifiManager.setConfigPortalTimeout(60);
  if (!wifiManager.autoConnect(mac_address.c_str()))
  {
    ESP.restart();
  }

  Serial.println("Đã kết nối Wi-Fi!");
}

void saveDeviceName(String name)
{
  strcpy(deviceInfo.name, name.c_str());

  EEPROM.put(0, deviceInfo);
  EEPROM.commit();
}

String getDeviceName()
{
  EEPROM.get(0, deviceInfo);

  return String(deviceInfo.name);
}