#include "config.hpp"
#include <Arduino.h>
#include <EEPROM.h>

namespace config
{
FLASH_DATA data;

void init()
{
    Serial.println("### CONFIG LOAD ###");
    EEPROM.begin(512);
    EEPROM.get<FLASH_DATA>(0, data);
    EEPROM.end();
}

void save()
{
    Serial.println("### CONFIG SAVE ###");
    EEPROM.put<FLASH_DATA>(0, data);
    EEPROM.commit();
}

}  // namespace config
