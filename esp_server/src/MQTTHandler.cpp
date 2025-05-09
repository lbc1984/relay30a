#include "MQTTHandler.h"
#include "WiFiHandler.h"
#include <Arduino_JSON.h>

PubSubClient client_mqtt(mqttSecure);

String topic = "";
const String url_lambda = "https://mqtt.sieuthitiendung.com/mqtt";
const char *url_send_message = "https://mqtt.sieuthitiendung.com/viber";

String user = "";
String pass = "";
String mqtt_url = "";
int mqtt_port = 8883;
int count_reconnect = 0;
bool isStarted = false;

void mqtt_setup()
{
  String json = fetchData();
  JSONVar data = JSON.parse(json);

  mqtt_url = String((const char *)data["url"]);
  user = String((const char *)data["user"]);
  pass = String((const char *)data["password"]);
  mqtt_port = String((const char *)data["port"]).toInt();

  client_mqtt.setServer(mqtt_url.c_str(), mqtt_port);
  client_mqtt.setCallback(mqttCallback);

  topic = "device/#";
}

void reconnect()
{
  while (!client_mqtt.connected())
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      ESP.restart();
    }

    Serial.println("Đang kết nối tới MQTT broker...");

    if (client_mqtt.connect("ESP_Server", user.c_str(), pass.c_str()))
    {
      Serial.println("Kết nối thành công!");
      client_mqtt.subscribe(topic.c_str());
      count_reconnect = 0;
    }
    else
    {
      Serial.print("Kết nối thất bại, mã lỗi: ");
      Serial.println(client_mqtt.state());
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

  String msg_viber = String(topic) + ": " + message;

  // Viber_Post(msg_viber);
}

String fetchData()
{
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

void Viber_Post(String message)
{
  String name = "Server Device";
  String receiver = "rc+eiS+JAFLl3CxRpznfIg==";

  JSONVar myJson;
  myJson["receiverId"] = receiver;
  myJson["messageText"] = message;
  myJson["botName"] = name;

  String jsonString = JSON.stringify(myJson);
  Serial.println("📨 JSON gửi đi: " + jsonString);

  httpClient.begin(clientSecure, url_send_message);
  httpClient.addHeader("Content-Type", "application/json");

  int result = httpClient.POST(JSON.stringify(myJson));
  Serial.println("Result viber: " + String(result));
  httpClient.end();
  clientSecure.stop();
}