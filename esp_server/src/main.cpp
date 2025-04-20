#include <Arduino.h>
#include "WiFiHandler.h"
#include "MQTTHandler.h"
#include "FirebaseHandler.h"
#include "Ticker.h"

Ticker timer1(controlDevice, 60000, 0, MILLIS);

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqtt_setup();
  setupFirebase();

  timer1.start();
}

void loop()
{
  if (!client_mqtt.connected())
  {
    reconnect();
  }
  client_mqtt.loop();

  timer1.update();
  delay(100);
}