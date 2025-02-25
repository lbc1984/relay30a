#include "MQTTHandler.h"
#include "WiFiHandler.h"
#include "ButtonHandler.h"
#include <Arduino_JSON.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);

String topic_root = "";
String topic_status = "";
String topic_switch = "";
String topic_name = "";
const String url_lambda = "https://2wbbwg9uw0.execute-api.ap-southeast-1.amazonaws.com/Prod/mqtt";

String user = "";
String pass = "";
String mqtt_url = "";
int mqtt_port = 8883;
int count_reconnect = 0;

void mqtt_setup()
{
  String json = fetchData();
  Serial.println("Response json: " + json);
  JSONVar data = JSON.parse(json);

  mqtt_url = String(data["url"]);
  user = String(data["user"]);
  pass = String(data["password"]);
  mqtt_port = String(data["port"]).toInt();

  Serial.println("MQTT URL: " + mqtt_url);
  Serial.println("MQTT User: " + user);
  Serial.println("MQTT Password: " + pass);
  Serial.println("MQTT Port: " + String(mqtt_port));

  client.setServer(mqtt_url.c_str(), mqtt_port);
  client.setCallback(mqttCallback);

  topic_root = "device/" + mac_address;
  topic_status = topic_root + "/status";
  topic_switch = topic_root + "/switch";
  topic_name = topic_root + "/name";

  Serial.println(topic_root);
  Serial.println(topic_status);
  Serial.println(topic_switch);
  Serial.println(topic_name);
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

    if (client.connect(mac_address.c_str(), user.c_str(), pass.c_str(), topic_status.c_str(), 1, true, "offline"))
    {
      Serial.println("Kết nối thành công!");
      client.subscribe(topic_status.c_str());
      client.subscribe(topic_switch.c_str());
      client.publish(topic_status.c_str(), "online", true);
      client.publish(topic_name.c_str(), deviceName.c_str());
      count_reconnect = 0;
    }
    else
    {
      Serial.print("Kết nối thất bại, mã lỗi: ");
      Serial.println(client.state());
      delay(5000);
      count_reconnect++;

      if(count_reconnect > 5){
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

  if (String(topic) == topic_switch)
    update_switch(message);
}

void pub_switch(String state)
{
  client.publish(topic_switch.c_str(), state.c_str());
}

String fetchData()
{
  clientSecure.connect(url_lambda, 443);
  httpClient.begin(clientSecure, url_lambda);
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