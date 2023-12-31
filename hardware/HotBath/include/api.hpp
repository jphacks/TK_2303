#pragma once
#include "etl/string_view.h"
#include <Arduino.h>

namespace api
{
typedef enum {
    BathNormal = 0,  // 通常
    BathIn,          // 入浴中
    BathInLongTime,  // 長時間入浴中
    BathOut,         // 退浴後から通常までの間
    BathDanger       // 危険
} Bath_Status;

class APILock
{
public:
    APILock();
    ~APILock();
};

void init();
bool post_sensor_data(float temperature, float pressure, float humidity);
bool set_bath_status(Bath_Status status);
Bath_Status get_bath_status();
bool post_wav_data_check_safe(const uint8_t* data, size_t data_size, bool& safe);
bool post_wav_data_check_alive(const uint8_t* data, size_t data_size, bool& alive);
bool get_latest_firmware_information(int& version, String& url);
bool post_alart(etl::string_view message);
}  // namespace api