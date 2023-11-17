#include "api.hpp"
#include "config.hpp"
#include "esp_log.h"
#include "etl/string.h"
#include "etl/string_stream.h"
#include "rootCA.hpp"
#include "secrets.hpp"
#include "utils.hpp"
#include "wifi.hpp"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

namespace api
{
static WiFiClientSecure client;
static char json_buf[512];
static etl::string<512> data_buf;
static StaticJsonDocument<512> doc;
static SemaphoreHandle_t xMutex = NULL;
static const char TAG[] = "API";

static Bath_Status bath_status = BathNormal;

static bool post(etl::string_view url, etl::string_view data);

APILock::APILock()
{
    xSemaphoreTakeRecursive(xMutex, portMAX_DELAY);
}

APILock::~APILock()
{
    xSemaphoreGiveRecursive(xMutex);
}

void init()
{
    // サーバーの証明書を設定
    client.setCACert(rootCA);

    xMutex = xSemaphoreCreateRecursiveMutex();
}

bool connect_client()
{
    if (!wifi::get_status()) {
        ESP_LOGE(TAG, "wifi not connected");
        return false;
    }

    if (!client.connect(HOST_URL, 443)) {
        ESP_LOGE(TAG, "client connection failed");
        client.stop();
        return false;
    }
    return true;
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

bool set_bath_status(Bath_Status status)
{
    APILock lock;

    doc.clear();
    bath_status = status;
    if (status == BathIn) {
        doc["status"] = "in";
    } else if (status == BathOut) {
        doc["status"] = "out";
    } else if (status == BathDanger) {
        doc["status"] = "danger";
    } else {
        return true;
    }
    serializeJson(doc, json_buf);

    return post("/bath/status", json_buf);
}

Bath_Status get_bath_status()
{
    return bath_status;
}

bool post(etl::string_view url, etl::string_view data)
{
    APILock lock;

    ESP_LOGI(TAG, "POST %s %s", url.data(), data.data());

    if (!connect_client()) {
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
    client.print(data_buf.data());
    client.println(data.data());

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

bool post_wav_data(const uint8_t* data, size_t data_size, bool& safe)
{
    APILock lock;

    ESP_LOGI(TAG, "POST WAV");

    if (!connect_client()) {
        return false;
    }

    data_buf.clear();
    etl::string_stream stream(data_buf);
    stream << "POST https://" HOST_URL << "/misc/check_safe"
           << " HTTP/1.1\r\n";
    stream << "Host: " HOST_URL "\r\n";
    stream << "Authorization: Bearer " << config::data.token << "\r\n";
    stream << "Content-Type: audio/wav\r\n";
    stream << "Content-Length: " << data_size << "\r\n";
    stream << "Keep-Alive: 30\r\n";
    stream << "Connection: keep-alive";
    stream << "\r\n\r\n";
    client.print(data_buf.data());

    size_t bufSize = 2048;
    while (data_size > 0) {
        int write_size = (data_size < bufSize) ? data_size : bufSize;
        client.write(data, write_size);
        data += write_size;
        data_size -= write_size;
        ESP_LOGE(TAG, "write %d", data_size);
    }
    client.print("\r\n");

    ESP_LOGI(TAG, "wait response");
    int64_t start = get_tick();
    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
        if (get_tick() - start > 10000) {
            ESP_LOGE(TAG, "timeout");
            client.stop();
            return false;
        }
    }
    ESP_LOGI(TAG, "received header");
    String line = client.readStringUntil('\n');
    ESP_LOGI(TAG, "received data: %s", line.c_str());

    deserializeJson(doc, line.c_str());

    safe = doc["safe"];

    client.stop();

    return true;
}

bool get_latest_firmware_information(int& version, String& url)
{
    APILock lock;

    ESP_LOGI(TAG, "GET LATEST VERSION");

    if (!connect_client()) {
        return false;
    }

    data_buf.clear();
    etl::string_stream stream(data_buf);
    stream << "GET https://" HOST_URL << "/firmware/versions/latest"
           << " HTTP/1.1\r\n";
    stream << "Host: " HOST_URL "\r\n";
    stream << "Authorization: Bearer " << config::data.token;
    stream << "\r\n\r\n";
    client.print(data_buf.data());

    while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            break;
        }
    }
    String line = client.readStringUntil('\n');
    ESP_LOGI(TAG, "received data: %s", line.c_str());

    deserializeJson(doc, line.c_str());

    version = doc["version"];
    url = doc["download_url"].as<String>();

    ESP_LOGI(TAG, "version: %d, url: %s", version, url.c_str());

    client.stop();

    return true;
}

}  // namespace api