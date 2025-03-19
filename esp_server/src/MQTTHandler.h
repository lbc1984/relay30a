#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern PubSubClient client;
extern String topic;
extern WiFiClientSecure clientSecure;
// extern WiFiClientSecure viberSecure;

void mqtt_setup();
void reconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);
String fetchData();
void Viber_Post(String message);
#endif
