#include "MQTTHandler.h"
#include "WiFiHandler.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

String topic_root = "device/" + mac_address;
String topic_status = topic_root + "/status";
String topic_switch = topic_root + "/switch";

void mqtt_setup()
{
  client.setServer("c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud", 8883);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Đang kết nối tới MQTT broker...");

    String topic_status = topic_root + "/status";
    espClient.setInsecure();

    if (client.connect(mac_address.c_str(), "lybaocuong", "1234@Abcd", topic_status.c_str(), 1, false, "offline"))
    {
      Serial.println("Kết nối thành công!");
      client.subscribe(topic_status.c_str());
      client.publish(topic_status.c_str(), "online", true);
    }
    else
    {
      Serial.print("Kết nối thất bại, mã lỗi: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
