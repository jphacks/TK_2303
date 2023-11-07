#include "wifi.hpp"
#include "secrets.hpp"
#include <WiFi.h>
#include "esp_log.h"

namespace wifi
{
static volatile bool status = false;

void update()
{
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    // WiFiのアクセスポイントに接続
    Serial.println("### WIFI INIT ###");
    Serial.printf("WiFi connecting to %s %s\n", MYSSID, MYPASS);
    WiFi.begin(MYSSID, MYPASS);
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        if (count > 25) {
            Serial.println("failed");
            status = false;
            return;
        }
        count++;
        Serial.print(".");
        delay(500);
    }

    // ESP32のIPアドレスを出力
    status = true;
    Serial.println("WiFi Connected.");
    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
}

bool get_status()
{
    return status;
};
}  // namespace wifi