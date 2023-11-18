#include "config.hpp"
#include <Arduino.h>
#include <EEPROM.h>
#include "esp_log.h"

namespace config
{
EEPROM_DATA data;
static const char TAG[] = "CONFIG";
__NOINIT_ATTR uint32_t cookie;
static bool first_boot = false;

void init()
{
    if (cookie != 0xdeadbeef) {
        cookie = 0xdeadbeef;
        first_boot = true;
        ESP_LOGI(TAG, "first boot");
    } else {
        ESP_LOGI(TAG, "reset boot");
    }

    ESP_LOGI(TAG, "load config size:%ld", sizeof(EEPROM_DATA));
    if (EEPROM.begin(1024)) {
        ESP_LOGI(TAG, "EEPROM.begin success");
    } else {
        ESP_LOGE(TAG, "EEPROM.begin failed");
    }
    EEPROM.get<EEPROM_DATA>(0, data);
    ESP_LOGI(TAG, "verify:0x%x", data.verify);
    if (data.verify != 0xdeadbeef) {
        factory_reset();
    }
}

void factory_reset()
{
    ESP_LOGI(TAG, "factory reset");
    data.verify = 0xdeadbeef;
    data.ssid[0] = '\0';
    data.pass[0] = '\0';
    data.token[0] = '\0';
    data.wifi_configured = false;
    data.token_configured = false;
    save();
}

void save()
{
    ESP_LOGI(TAG, "save config");
    EEPROM.put<EEPROM_DATA>(0, data);
    if (EEPROM.commit()) {
        ESP_LOGI(TAG, "EEPROM.commit success");
    } else {
        ESP_LOGE(TAG, "EEPROM.commit failed");
    }
}

bool is_first_boot()
{
    return first_boot;
    // return true;
}

}  // namespace config
