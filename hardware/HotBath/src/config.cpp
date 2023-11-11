#include "config.hpp"
#include <Arduino.h>
#include <EEPROM.h>

namespace config
{
FLASH_DATA data;

void init()
{
    Serial.println("### CONFIG LOAD ###");
    EEPROM.begin(sizeof(FLASH_DATA));
    EEPROM.get<FLASH_DATA>(0, data);
    EEPROM.end();
    Serial.println("load successfully");
}

void save()
{
    Serial.println("### CONFIG SAVE ###");
    EEPROM.put<FLASH_DATA>(0, data);
    EEPROM.commit();
    Serial.println("saved");
}

}  // namespace config
