#include "online_update.hpp"

#include "WiFi.h"

#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "rootCA.hpp"
#include "esp_log.h"
#include "api.hpp"
#include "secrets.hpp"
#include "config.hpp"
#include <Arduino.h>

namespace update
{
static const char TAG[] = "OTA";
static long contentLength = 0;
static long receivedBytes = 0;
static int progress = 0;

static void update(String url);

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName)
{
    return header.substring(strlen(headerName.c_str()));
}

esp_err_t _http_event_handler(esp_http_client_event_t* evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        Serial.println("HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        Serial.println("HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        Serial.println("HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s\n", evt->header_key, evt->header_value);
        if (String(evt->header_key) == "Content-Length") {
            contentLength = atol(evt->header_value);
        }
        break;
    case HTTP_EVENT_ON_DATA:
        receivedBytes += evt->data_len;
        // println every 10%
        if (progress != (int)(receivedBytes * 10 / contentLength)) {
            progress = (int)(receivedBytes * 10 / contentLength);
            Serial.printf("HTTP_EVENT_ON_DATA, progress=%d%%\n", progress * 10);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        Serial.println("HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        Serial.println("HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void check()
{
    Serial.println("Check OTA update");

    int version;
    String url;
    api::get_latest_firmware_information(version, url);

    if (version > config::PROGRAM_VERSION) {
        update(url);
    } else {
        Serial.println("No update available");
    }
}

void update(String url)
{
    Serial.println("Start OTA update");

    esp_http_client_config_t config = {
        .url = url.c_str(),
        .cert_pem = api::rootCA,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };

    Serial.printf("Attempting to download update from %s\n", config.url);
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        Serial.println("OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        Serial.println("Firmware upgrade failed");
    }
}

void rollback_check()
{
    const esp_partition_t* running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                Serial.println("App is valid.");
            } else {
                Serial.println("Rollback check was failed");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }
}
}  // namespace update