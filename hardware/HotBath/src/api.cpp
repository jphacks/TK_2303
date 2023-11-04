#include "api.hpp"
#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/string_view.h"
#include "rootCA.hpp"
#include "secrets.hpp"
#include "wifi.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

namespace api
{
static WiFiClientSecure client;
static char json_buf[512];
static etl::string<512> data_buf;
static StaticJsonDocument<100> doc;
static SemaphoreHandle_t xMutex = NULL;

static bool post(etl::string_view url, etl::string_view data);

class APILock
{
public:
    APILock()
    {
        xSemaphoreTakeRecursive(xMutex, portMAX_DELAY);
    }
    ~APILock()
    {
        xSemaphoreGiveRecursive(xMutex);
    }
};

void init()
{
    // サーバーの証明書を設定
    client.setCACert(rootCA);

    xMutex = xSemaphoreCreateRecursiveMutex();
}

bool post_sensor_data(int temperature, int humidity, int co2)
{
    APILock lock;

    doc.clear();
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["co2"] = co2;
    serializeJson(doc, json_buf);

    return post("/sensors/9910", json_buf);
}

bool post(etl::string_view url, etl::string_view data)
{
    Serial.printf("### API POST %s ###\n", url.data());
    if (!wifi::get_status()) {
        Serial.println("wifi is not connected");
        return false;
    }

    if (!client.connect(HOST_URL, 443)) {
        Serial.println("connection failed");
        client.stop();
        return false;
    }

    data_buf.clear();
    etl::string_stream stream(data_buf);
    stream << "POST https://" HOST_URL << url.data() << " HTTP/1.1\r\n";
    stream << "Host: " HOST_URL "\r\n";
    stream << "Content-Type: application/json\r\n";
    stream << "Content-Length: " << data.length();
    stream << "\r\n\r\n";
    stream << data.data() << "\r\n";

    client.print(data_buf.data());

    Serial.println("request sent");
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
    }
    String line = client.readStringUntil('\n');
    Serial.println(line);

    client.stop();
    Serial.println("closed connection");

    return true;
}
}  // namespace api