#include "config.hpp"
#include <Arduino.h>
#include <EEPROM.h>
#include "esp_log.h"

namespace config
{
EEPROM_DATA data;
static const char TAG[] = "CONFIG";

void init()
{
    ESP_LOGI(TAG, "load config");
    EEPROM.begin(1024);
    EEPROM.get<EEPROM_DATA>(0, data);
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
    EEPROM.commit();
}

}  // namespace config
