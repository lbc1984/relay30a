#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern String topic_status;
extern String topic_switch;

void mqtt_setup();

void reconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);
void pub_switch(String state);
#endif
