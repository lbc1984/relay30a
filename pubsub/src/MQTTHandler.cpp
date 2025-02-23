#include "MQTTHandler.h"
#include "WiFiHandler.h"
#include "ButtonHandler.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

String topic_root = "";
String topic_status = "";
String topic_switch = "";
String topic_name = "";

const char *user = "lybaocuong";
const char *pass = "1234@Abcd";
const char *mqtt_url = "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud";
const int mqtt_port = 8883;

void mqtt_setup()
{
  client.setServer(mqtt_url, mqtt_port);
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

    if (client.connect(mac_address.c_str(), user, pass, topic_status.c_str(), 1, true, "offline"))
    {
      Serial.println("Kết nối thành công!");
      client.subscribe(topic_status.c_str());
      client.subscribe(topic_switch.c_str());
      client.publish(topic_status.c_str(), "online", true);
      client.publish(topic_name.c_str(), deviceName.c_str());
    }
    else
    {
      Serial.print("Kết nối thất bại, mã lỗi: ");
      Serial.println(client.state());
      delay(5000);
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