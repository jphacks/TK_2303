#pragma once
#include <freertos/FreeRTOS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

namespace sensors {
const int BME_CS = 15;
const int BME_SCK = 14;
const int BME_MOSI = 13;
const int BME_MISO = 12;
const int AD_PIN = 34;

Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

float temp = 0;
float humid = 0;
float pressure = 0;
int co2 = 0;

void sensorTask(void *param) {
  while (true) {
    temp = bme.readTemperature();
    humid = bme.readHumidity();
    pressure = bme.readPressure() / 100.0;
    co2 = analogReadMilliVolts(AD_PIN);
    vTaskDelay(100);
  }
}

void setup() {
  pinMode(AD_PIN, ANALOG);
  bme.begin();
  xTaskCreatePinnedToCore(sensorTask, "SensorTask", 0x800, NULL, 2, NULL, 1);
}

float getTemp() {
  return temp;
}

float getHumid() {
  return humid;
}

float getPressure() {
  return pressure;
}

int getCO2() {
  return co2;
}
}