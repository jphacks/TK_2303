#pragma once
#include <Arduino.h>
namespace config
{
constexpr int PROGRAM_VERSION = 0;
constexpr int BOARD_REVISION = 0;

constexpr int MAX_TOKEN_LENGTH = 256;

struct EEPROM_DATA {
    uint32_t verify;
    char ssid[64];
    char pass[64];
    char token[MAX_TOKEN_LENGTH];
    uint32_t wifi_configured;
    uint32_t token_configured;
};

extern EEPROM_DATA data;

void init();
void save();
void factory_reset();

}  // namespace config