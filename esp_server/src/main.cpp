#include <Arduino.h>
#include "WiFiHandler.h"
#include "MQTTHandler.h"
#include <Firebase_ESP_Client.h>

// Firebase config
#define API_KEY "AIzaSyCXSu97M3_ONeEV5GL2bn9MheySzvjTAzQ"
#define DATABASE_URL "https://mqtt-d8e66-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Khai báo các đối tượng Firebase
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;

void streamCallback(FirebaseStream data)
{
  Serial.println("📥 Stream event:");
  Serial.print("📍 Path: ");
  Serial.println(data.dataPath());

  Serial.print("🧾 Data: ");
  Serial.println(data.stringData());
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println("⚠️ Mất kết nối stream, thử kết nối lại...");
  }
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqtt_setup();

  // Cấu hình Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Xác thực bằng email/password
  auth.user.email = "lybaocuong@gmail.com";
  auth.user.password = "1234@Nasi";

  // Bắt đầu Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Bắt đầu stream từ đường dẫn trong database
  if (!Firebase.RTDB.beginStream(&stream, "/schedules"))
  {
    Serial.print("❌ Lỗi bắt đầu stream: ");
    Serial.println(stream.errorReason());
  }
  else
  {
    Serial.println("✅ Bắt đầu stream thành công!");
  }
  {
    Serial.print("❌ Lỗi stream: ");
    Serial.println(stream.errorReason());
  }

  // Callback khi có dữ liệu
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
