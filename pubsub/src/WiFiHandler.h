#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

extern String mac_address;
extern String deviceName;
extern WiFiClientSecure clientSecure;
extern HTTPClient httpClient;

void setup_wifi();

#endif
