#include "FirebaseHandler.h"
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "MQTTHandler.h"
#include <time.h>

const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
std::map<String, FirebaseJson *> scheduleMap;
std::vector<DeviceAction> filteredActions;

tm getDateTime()
{
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo))
    {
        delay(1000);
    }

    return timeinfo;
}

void streamCallback(FirebaseStream data)
{
    Serial.println("Đã nhận dữ liệu mới từ Firebase");

    if (data.dataPath() == "/")
    {
        FirebaseJson *json = data.to<FirebaseJson *>();
        size_t count = json->iteratorBegin();

        for (size_t i = 0; i < count; i++)
        {
            FirebaseJson::IteratorValue value = json->valueAt(i);
            String deviceId = value.key;

            if (deviceId.length() > 10)
            {
                scheduleMap[deviceId] = new FirebaseJson();
                scheduleMap[deviceId]->setJsonData(value.value);
            }
        }

        json->iteratorEnd();
        json->clear();
    }
    else
    {
        String path = data.dataPath();
        String deviceId = path.substring(1);

        FirebaseJson *json = data.to<FirebaseJson *>();

        if (scheduleMap.find(deviceId) != scheduleMap.end())
        {
            FirebaseJson *oldJson = scheduleMap[deviceId];
            if (oldJson && oldJson != json)
            {
                delete oldJson;
            }
        }

        scheduleMap[deviceId] = new FirebaseJson();
        scheduleMap[deviceId]->setJsonData(json->raw());
    }
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
        Serial.println("⚠️ Stream bị timeout, đang kết nối lại...");
}

void setupFirebase()
{
    Firebase.reset(&config);
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    auth.user.email = EMAIL;
    auth.user.password = PASSWORD;

    Serial.println("Connecting to Firebase...");

    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    if (!Firebase.RTDB.beginStream(&fbdo, "/schedule"))
    {
        Serial.printf("Stream lỗi: %s\n", fbdo.errorReason().c_str());
        return;
    }

    Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);

    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");
}

void filterActionsByDay(const String &targetDay)
{
    filteredActions.clear();

    for (auto item : scheduleMap)
    {
        String deviceId = item.first;
        FirebaseJson *json = item.second;

        String action, time, day;
        FirebaseJsonData jsonEvent;
        FirebaseJsonArray jsonArray;

        json->get(jsonEvent, "events");
        jsonEvent.getArray(jsonArray);

        for (size_t i = 0; i < jsonArray.size(); i++)
        {
            FirebaseJsonData dataEvent, data;
            FirebaseJson jsonData;

            jsonArray.get(dataEvent, i);
            jsonData.setJsonData(dataEvent.stringValue);

            jsonData.get(data, "action");
            action = data.stringValue;

            jsonData.get(data, "time");
            time = data.stringValue;

            jsonData.get(data, "day");
            day = data.stringValue;

            if (day == targetDay)
            {
                DeviceAction deviceAction = {deviceId, action, time, day};
                filteredActions.push_back(deviceAction);
            }
        }
    }
}

void controlDevice()
{
    tm now = getDateTime();
    filterActionsByDay(String(now.tm_wday + 1));

    if (filteredActions.empty())
    {
        Serial.println("Không có hành động nào trong filteredActions!");
    }
    else
    {
        for (size_t i = 0; i < filteredActions.size(); ++i)
        {
            DeviceAction action = filteredActions[i];

            String hour = String(now.tm_hour);
            if (hour.length() < 2)
                hour = "0" + hour;

            String minute = String(now.tm_min);
            if (minute.length() < 2)
                minute = "0" + minute;

            String time = hour + ":" + minute;

            if (action.time == time)
            {
                String device_id = formatDeviceId(action.deviceId);
                String toppic_switch = "device/" + device_id + "/switch";
                String on_off = action.action == "ON" ? "1" : "0";
                
                Serial.println(toppic_switch);
                Serial.printf("Gửi lệnh %s đến thiết bị %s, time: %s\n", on_off.c_str(), device_id.c_str(), action.time);
                Serial.println(client_mqtt.state());
                client_mqtt.publish(toppic_switch.c_str(), on_off.c_str());
            }
        }
    }
}

String formatDeviceId(String deviceId)
{
    deviceId.toUpperCase();

    String formatted = "";
    for (int i = 0; i < 12; i += 2)
    {
        formatted += deviceId.substring(i, i + 2);
        if (i < 10)
            formatted += ":";
    }

    return formatted;
}
