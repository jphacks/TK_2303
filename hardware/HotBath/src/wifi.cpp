#include "wifi.hpp"
#include <WiFi.h>
#include "esp_log.h"
#include "config.hpp"

namespace wifi
{
static const char TAG[] = "WIFI";

void update()
{
    if (get_status()) {
        return;
    }
    if (config::data.wifi_configured == false) {
        ESP_LOGI(TAG, "SSID/PASS not configured");
        return;
    }
    // WiFiのアクセスポイントに接続
    ESP_LOGI(TAG, "connecting to %s %s", config::data.ssid, config::data.pass);
    WiFi.begin(config::data.ssid, config::data.pass);
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (count > 35) {
            ESP_LOGE(TAG, "connect failed");
            return;
        }
        count++;
        vTaskDelay(500);
    }

    // ESP32のIPアドレスを出力
    ESP_LOGI(TAG, "connected IP = %s", WiFi.localIP().toString().c_str());
}

bool get_status()
{
    return (WiFi.status() == WL_CONNECTED);
};
}  // namespace wifi