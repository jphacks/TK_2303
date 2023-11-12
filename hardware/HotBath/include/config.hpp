#pragma once
#include <Arduino.h>

#define PROGRAM_VERSION 0
#define BOARD_REVISION 0

#define MAX_TOKEN_LENGTH 256
namespace config
{

struct EEPROM_DATA {
    uint32_t verify;
    char ssid[128];
    char pass[128];
    char token[MAX_TOKEN_LENGTH];
    uint32_t wifi_configured;
    uint32_t token_configured;
};

extern EEPROM_DATA data;

void init();
void save();
void factory_reset();

}  // namespace config