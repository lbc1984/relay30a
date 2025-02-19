#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>

const char *mqtt_server = "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud";
const int mqtt_port = 8883; // 1883 (không bảo mật), 8883 (TLS)
const char *mqtt_user = "lybaocuong";
const char *mqtt_password = "1234@Abcd";

WiFiClientSecure espClient;
PubSubClient client(espClient);

const int buttonPin = D1;            // Chân D1 trên ESP8266
volatile bool buttonPressed = false; // Biến lưu trạng thái nút nhấn
bool onoff = false;

String deviceName = "";
String mac_address = WiFi.macAddress();
String topic = "device/" + mac_address;

void IRAM_ATTR buttonISR()
{
  buttonPressed = true;
}

void setup_wifi()
{
  WiFiManager wifiManager;
  WiFiManagerParameter custom_device_name("device_name", "Enter Device Name", deviceName.c_str(), 20);
  wifiManager.addParameter(&custom_device_name);

  if (!wifiManager.autoConnect("ESP8266-AP"))
  {
    Serial.println("Không thể kết nối Wi-Fi!");
    delay(3000);
    ESP.restart();
  }

  deviceName = custom_device_name.getValue();
  Serial.print("Tên thiết bị: ");
  Serial.println(deviceName);
  Serial.println("Đã kết nối Wi-Fi!");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Nhận tin nhắn từ topic: ");
  Serial.println(topic);
  Serial.print("Nội dung: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Đang kết nối tới MQTT broker...");

    String topic_status = topic + "/status";

    espClient.setInsecure();

    if (client.connect(mac_address.c_str(), mqtt_user, mqtt_password, topic_status.c_str(), 1, true, "offline"))
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

void setup()
{
  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (buttonPressed)
  {
    bool status_button = digitalRead(buttonPin);
    delay(50);

    if (status_button == HIGH)
    {
      buttonPressed = false;
      onoff = !onoff;
      Serial.println("Nút được nhấn!");
      String topic_switch = topic + "/switch";

      client.publish(topic_switch.c_str(), String(onoff).c_str(), true);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}