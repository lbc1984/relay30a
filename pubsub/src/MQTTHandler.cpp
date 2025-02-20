#include "MQTTHandler.h"
#include "WiFiHandler.h"
#include "ButtonHandler.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

String topic_root = "device/" + mac_address;
String topic_status = topic_root + "/status";
String topic_switch = topic_root + "/switch";

const char *user = "lybaocuong";
const char *pass = "1234@Abcd";
const char *mqtt_url = "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud";
const int mqtt_port = 8883;

void mqtt_setup()
{
  client.setServer(mqtt_url, mqtt_port);
  client.setCallback(mqttCallback);
}

void reconnect()
{
  espClient.setInsecure();

  while (!client.connected())
  {
    Serial.println("Đang kết nối tới MQTT broker...");

    if (client.connect(mac_address.c_str(), user, pass, topic_status.c_str(), 1, false, "offline"))
    {
      Serial.println("Kết nối thành công!");
      client.subscribe(topic_status.c_str());
      client.subscribe(topic_switch.c_str());
      client.publish(topic_status.c_str(), "online");
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
  Serial.print("Nhận tin nhắn từ topic: ");
  Serial.println(topic);
  Serial.print("Nội dung: ");
  String message = String((char *)payload).substring(0, length);

  if (String(topic) == topic_switch)
    update_switch(message);
}

void pub_switch(String state){
  client.publish(topic_switch.c_str(), state.c_str());
}