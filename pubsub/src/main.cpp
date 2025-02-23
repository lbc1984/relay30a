#include <Arduino.h>
#include "WiFiHandler.h"
#include "MQTTHandler.h"
#include "ButtonHandler.h"

bool isStarted = false;

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqtt_setup();
  setup_button();

  pub_switch("off");
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  handle_button();

  if(isStarted == false)
  {
    set_switch(0);
    isStarted = true;
  } 
}
