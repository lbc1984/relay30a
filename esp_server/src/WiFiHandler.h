#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

extern String mac_address, deviceName;
extern WiFiClientSecure clientSecure, viberSecure;
extern WiFiClientSecure viberSecure;
extern HTTPClient httpClient;

void setup_wifi();

#endif
