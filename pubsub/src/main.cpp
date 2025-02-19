#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

// Thông tin MQTT Broker
const char *mqtt_server = "c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud";
const int mqtt_port = 8883; // 1883 (không bảo mật), 8883 (TLS)
const char *mqtt_user = "lybaocuong";
const char *mqtt_password = "1234@Abcd1";

WiFiClient espClient;
PubSubClient client(espClient);

// Định nghĩa chân nút và biến trạng thái
const int buttonPin = D1;            // Chân D1 trên ESP8266
volatile bool buttonPressed = false; // Biến lưu trạng thái nút nhấn

// Biến lưu tên thiết bị
String deviceName = "";

// Hàm ngắt xử lý khi nút được nhấn
void IRAM_ATTR buttonISR()
{
  buttonPressed = true;
}

// Hàm kết nối Wi-Fi sử dụng WiFiManager
void setup_wifi()
{
  WiFiManager wifiManager;

  // Thêm trường nhập tên thiết bị (device name)
  WiFiManagerParameter custom_device_name("device_name", "Enter Device Name", deviceName.c_str(), 20); // Tên, nhãn, giá trị mặc định và độ dài tối đa

  // Thêm tham số tùy chỉnh vào WiFiManager
  wifiManager.addParameter(&custom_device_name);

  // Nếu không kết nối Wi-Fi, WiFiManager sẽ tạo một hotspot để người dùng kết nối
  if (!wifiManager.autoConnect("ESP8266-AP"))
  {
    Serial.println("Không thể kết nối Wi-Fi!");
    delay(3000);
    ESP.restart(); // Khởi động lại nếu không thể kết nối
  }

  // Lấy giá trị tên thiết bị từ trường input
  deviceName = custom_device_name.getValue();
  Serial.print("Tên thiết bị: ");
  Serial.println(deviceName);

  Serial.println("Đã kết nối Wi-Fi!");
}

// Callback nhận tin nhắn từ MQTT Broker
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

    String mac_address = WiFi.macAddress();
    String topic_status = "device/" + mac_address + "/status";

    if (client.connect(mac_address.c_str(), mqtt_user, mqtt_password, topic_status.c_str(), 1, true, "offline"))
    {
      Serial.println("Kết nối thành công!");

      client.publish(topic_status.c_str(), "online", true);
      client.subscribe("esp8266/receive");
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
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING); // Sử dụng ngắt cho nút nhấn (khi nhấn nút thì trạng thái sẽ FALLING)
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Kiểm tra xem nút có được nhấn không (ngắt đã được kích hoạt)
  if (buttonPressed)
  {
    buttonPressed = false; // Đặt lại trạng thái nút nhấn
    Serial.println("Nút được nhấn!");

    // Gửi thông tin lên MQTT tùy thuộc vào trạng thái
    client.publish("esp8266/send", "on");
    delay(500); // Tạm dừng một chút để tránh gửi quá nhanh

    // Nếu cần gửi thêm thông tin "off", có thể làm như sau:
    // client.publish("esp8266/send", "off");

    // Nếu muốn bật/tắt LED để kiểm tra
    digitalWrite(LED_BUILTIN, HIGH); // Bật LED
    delay(1000);                     // Giữ LED bật 1 giây
    digitalWrite(LED_BUILTIN, LOW);  // Tắt LED
  }
}
