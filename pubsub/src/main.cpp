#include <Arduino.h>
#include "WiFiHandler.h"
#include "MQTTHandler.h"
#include "ButtonHandler.h"

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqtt_setup();
  setup_button();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  handle_button();
}
