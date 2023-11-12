#include "led.hpp"

#include "ble.hpp"
#include <Arduino.h>

#define RED_LED_PIN 40
#define BLUE_LED_PIN 36
#define SW_PIN 37

namespace led
{
static LED_Mode mode_ = Normal;
static int count_ = 0;
static bool direction_ = false;

void led_task(void* pvParameters)
{
    while (1) {
        switch (mode_) {
        case Normal:
            analogWrite(RED_LED_PIN, 0);
            analogWrite(BLUE_LED_PIN, 255 - count_);
            if (direction_) {
                count_ -= 10;
            } else {
                count_ += 10;
            }
            break;
        case Pairing:
            count_ += 25;
            if (count_ < 128) {
                analogWrite(RED_LED_PIN, 255);
                analogWrite(BLUE_LED_PIN, 0);
            } else {
                analogWrite(RED_LED_PIN, 0);
                analogWrite(BLUE_LED_PIN, 255);
            }
            if (count_ > 255) {
                count_ = 0;
            }
            break;
        default:
            break;
        }

        if (count_ < 0) {
            direction_ = false;
            count_ = 0;
        } else if (count_ > 255) {
            direction_ = true;
            count_ = 255;
        }

        if (digitalRead(SW_PIN) == LOW) {
            ble::open_request();
        }

        vTaskDelay(50);
    }
}

void init()
{
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    pinMode(SW_PIN, INPUT_PULLUP);

    analogWrite(RED_LED_PIN, 255);
    analogWrite(BLUE_LED_PIN, 255);

    xTaskCreatePinnedToCore(led_task, "led_task", 4096, NULL, 5, NULL, 0);
}

void set_mode(LED_Mode mode)
{
    mode_ = mode;
    count_ = 0;
    direction_ = false;
}
}  // namespace led