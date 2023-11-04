#include "api.hpp"
#include "wifi.hpp"
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    wifi::update();
    api::init();
}

void loop()
{
    wifi::update();
    api::postSensorData(1, 2, 3);
    delay(10000);
}