#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <HTTPClient.h>

extern String mac_address;
extern String deviceName;
extern WiFiClientSecure clientSecure;
extern WiFiClientSecure mqttSecure;
extern HTTPClient httpClient;

void setup_wifi();

#endif
