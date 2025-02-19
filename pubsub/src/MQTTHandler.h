#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern String topic_status;
extern String topic_switch;

void mqtt_setup();
void reconnect();

#endif
