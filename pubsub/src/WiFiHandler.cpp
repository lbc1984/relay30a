#include "WiFiHandler.h"

String mac_address = "";
String deviceName = "";

void setup_wifi()
{
  WiFiManager wifiManager;
  // WiFiManagerParameter custom_device_name("device_name", "Enter Device Name", deviceName.c_str(), 20);
  // wifiManager.addParameter(&custom_device_name);

  if (!wifiManager.autoConnect("ESP8266-AP"))
  {
    Serial.println("Không thể kết nối Wi-Fi!");
    delay(3000);
    ESP.restart();
  }

  mac_address = WiFi.macAddress(); // Gán địa chỉ MAC sau khi kết nối thành công

  // // Lấy tên thiết bị sau khi người dùng nhập
  // deviceName = custom_device_name.getValue();

  // if (deviceName == "")
  // {
  //   Serial.println("Tên thiết bị không được để trống!");
  //   delay(3000);
  //   wifiManager.resetSettings();
  //   ESP.restart();
  // }

  // Serial.print("Tên thiết bị: ");
  // Serial.println(deviceName);
  Serial.println("Đã kết nối Wi-Fi!");
}
