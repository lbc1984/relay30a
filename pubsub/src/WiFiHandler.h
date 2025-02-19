#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

extern String mac_address;
extern String deviceName;

void setup_wifi();

#endif
