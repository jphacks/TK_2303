#include "api.hpp"
#include "config.hpp"
#include "esp_log.h"
#include "etl/string.h"
#include "etl/string_stream.h"
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
static const char TAG[] = "API";

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

bool post_sensor_data(float temperature, float pressure, float humidity)
{
    APILock lock;

    doc.clear();
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["pressure"] = pressure;
    serializeJson(doc, json_buf);

    return post("/bath/sensors", json_buf);
}

bool post_bath_status(Bath_Status status)
{
    APILock lock;

    doc.clear();
    if (status == BathIn) {
        doc["status"] = "in";
    } else if (status == BathOut) {
        doc["status"] = "out";
    } else if (status == BathDanger) {
        doc["status"] = "danger";
    } else {
        doc["status"] = "unknown";
    }
    serializeJson(doc, json_buf);

    return post("/bath/status", json_buf);
}

bool post(etl::string_view url, etl::string_view data)
{
    ESP_LOGI(TAG, "POST %s %s", url.data(), data.data());
    if (!wifi::get_status()) {
        ESP_LOGE(TAG, "wifi not connected");
        return false;
    }

    if (!client.connect(HOST_URL, 443)) {
        ESP_LOGE(TAG, "client connection failed");
        client.stop();
        return false;
    }

    data_buf.clear();
    etl::string_stream stream(data_buf);
    stream << "POST https://" HOST_URL << url.data() << " HTTP/1.1\r\n";
    stream << "Host: " HOST_URL "\r\n";
    stream << "Authorization: Bearer " << config::data.token << "\r\n";
    stream << "Content-Type: application/json\r\n";
    stream << "Content-Length: " << data.length();
    stream << "\r\n\r\n";
    stream << data.data() << "\r\n";

    client.print(data_buf.data());

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }
    String line = client.readStringUntil('\n');
    ESP_LOGI(TAG, "received data: %s", line.c_str());

    client.stop();

    return true;
}
}  // namespace api