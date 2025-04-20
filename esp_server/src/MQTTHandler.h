#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>

extern PubSubClient client_mqtt;
extern String topic;

void mqtt_setup();
void reconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);
String fetchData();
void Viber_Post(String message);
#endif
