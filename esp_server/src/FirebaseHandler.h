// FirebaseHandler.h
#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#include <Firebase_ESP_Client.h>

#include <map>

#define API_KEY "AIzaSyCXSu97M3_ONeEV5GL2bn9MheySzvjTAzQ"
#define DATABASE_URL "https://mqtt-d8e66-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define EMAIL "lbc1984@gmail.com"
#define PASSWORD "123456"

struct DeviceAction
{
    String deviceId;
    String action;
    String time;
    String day;
};

extern std::map<String, FirebaseJson *> scheduleMap;
extern std::vector<DeviceAction> filteredActions;

void setupFirebase();
void streamCallback(FirebaseStream data);
void streamTimeoutCallback(bool timeout);
void filterActionsByDay(const String &targetDay);
void controlDevice();
String formatDeviceId(String deviceId);
#endif
