#pragma once

#define PROGRAM_VERSION "1.0.0"

namespace config
{

struct FLASH_DATA {
    char ssid[60];
    char pass[60];
};

extern FLASH_DATA data;

void init();
void save();

}  // namespace config