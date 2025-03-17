#include "MQTTHandler.h"
#include "WiFiHandler.h"
#include <Arduino_JSON.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);

String topic = "";
const String url_lambda = "https://mqtt.sieuthitiendung.com/mqtt";

String user = "";
String pass = "";
String mqtt_url = "";
int mqtt_port = 8883;
int count_reconnect = 0;
bool isStarted = false;

void mqtt_setup()
{
  String json = fetchData();
  Serial.println("Response json: " + json);
  JSONVar data = JSON.parse(json);

  mqtt_url = String(data["url"]);
  user = String(data["user"]);
  pass = String(data["password"]);
  mqtt_port = String(data["port"]).toInt();

  client.setServer(mqtt_url.c_str(), mqtt_port);
  client.setCallback(mqttCallback);

  topic = "device/#";
}

void reconnect()
{
  espClient.setInsecure();

  while (!client.connected())
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      ESP.restart();
    }

    Serial.println("Đang kết nối tới MQTT broker...");

    if (client.connect("ESP_Server", user.c_str(), pass.c_str()))
    {
      Serial.println("Kết nối thành công!");
      client.subscribe(topic.c_str());
      count_reconnect = 0;
    }
    else
    {
      Serial.print("Kết nối thất bại, mã lỗi: ");
      Serial.println(client.state());
      delay(5000);
      count_reconnect++;

      if (count_reconnect > 5)
      {
        ESP.restart();
      }
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String message = String((char *)payload).substring(0, length);
  Serial.print("Nhận tin nhắn từ topic: ");
  Serial.print(topic);
  Serial.println(message);

  if (String(topic) == topic_switch && isStarted == true)
    update_switch(message);
  else if (isStarted == false)
  {
    set_switch(true);
    isStarted = true;
  }
  else if (String(topic) == topic_reset && message == "lbc")
  {
    wifiManager.resetSettings();
    ESP.restart();
  }
}

String fetchData()
{
  clientSecure.connect(url_lambda, 443);
  httpClient.begin(clientSecure, url_lambda);
  httpClient.addHeader("X-Device-MAC", mac_address);

  int httpCode = httpClient.GET();
  String payload = "";

  if (httpCode > 0)
  {
    payload = httpClient.getString();
    Serial.println("Response: " + payload);
  }
  else
  {
    Serial.println("HTTP Error: " + String(httpCode));
  }

  httpClient.end();
  clientSecure.stop();

  return payload;
}