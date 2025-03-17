#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern String topic;

void mqtt_setup();
void reconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);
String fetchData();
#endif
