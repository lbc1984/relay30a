#include <Arduino.h>
#include "WiFiHandler.h"
#include "MQTTHandler.h"

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqtt_setup();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
