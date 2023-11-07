#include "Wifi.h"
#include "Mic.h"

#include "Sensors.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

const int MIC_CLK = 5;
const int MIC_DATA = 35;

#define SSID "bld2-guest"
#define PASS "MatsumotoWay"

// #define SSID "TP-Link_0338"
// #define PASS "09968035"

char server_json[130];
StaticJsonDocument<100> doc;
HTTPClient httpClient;

bool state = false;
int count = 0;

void mainTask(void *param) {
  while (true) {
    if (count > 20) {
      /* http送信*/
      wifi::setup(SSID, PASS);

      doc.clear();
      doc["temperature"] = sensors::getTemp();
      doc["humidity"] = sensors::getHumid();
      doc["co2"] = sensors::getCO2();
      serializeJson(doc, server_json);

      httpClient.begin("http://bathcare.tsubame.workers.dev/sensors/9910");
      httpClient.addHeader("Content-Type", "application/json");
      //  POSTしてステータスコードを取得する
      int status_code = httpClient.POST((uint8_t *)server_json, strlen(server_json));
      if (status_code == 200) {
        Serial.println("[*] Sensors POST Success");
      } else {
        Serial.println("[*] Sensors POST Failed");
      }
      httpClient.end();

      if (sensors::getHumid() > 70 && !state) {
        state = true;
        doc.clear();
        doc["status"] = "in";
        serializeJson(doc, server_json);
        httpClient.begin("http://bathcare.tsubame.workers.dev/status/9910");
        httpClient.addHeader("Content-Type", "application/json");
        //  POSTしてステータスコードを取得する
        int status_code = httpClient.POST((uint8_t *)server_json, strlen(server_json));
        if (status_code == 200) {
          Serial.println("[*] Status in POST Success");
        } else {
          Serial.println("[*] Status in POST Failed");
        }
        httpClient.end();

        mic::big_sound = false;
      }

      if (sensors::getHumid() <= 70 && state) {
        state = false;
        doc.clear();
        doc["status"] = "out";
        serializeJson(doc, server_json);
        httpClient.begin("http://bathcare.tsubame.workers.dev/status/9910");
        httpClient.addHeader("Content-Type", "application/json");
        //  POSTしてステータスコードを取得する
        int status_code = httpClient.POST((uint8_t *)server_json, strlen(server_json));
        if (status_code == 200) {
          Serial.println("[*] Status out POST Success");
        } else {
          Serial.println("[*] Status out POST Failed");
        }
        httpClient.end();
      }
      count = 0;
    }
    count++;

    if (state && mic::big_sound) {
      Serial.println("Big sound!");

      mic::big_sound = false;

      doc.clear();
      doc["status"] = "danger";
      serializeJson(doc, server_json);
      httpClient.begin("http://bathcare.tsubame.workers.dev/status/9910");
      httpClient.addHeader("Content-Type", "application/json");
      //  POSTしてステータスコードを取得する
      int status_code = httpClient.POST((uint8_t *)server_json, strlen(server_json));
      if (status_code == 200) {
        Serial.println("[*] Status danger POST Success");
      } else {
        Serial.println("[*] Status danger POST Failed");
      }
      httpClient.end();

      doc.clear();
      doc["message"] = "入浴中に大きな音を検知しました";
      serializeJson(doc, server_json);
      httpClient.begin("http://bathcare.tsubame.workers.dev/alart/9910");
      httpClient.addHeader("Content-Type", "application/json");
      //  POSTしてステータスコードを取得する
      status_code = httpClient.POST((uint8_t *)server_json, strlen(server_json));
      if (status_code == 200) {
        Serial.println("[*] Status alart POST Success");
      } else {
        Serial.println("[*] Status alart POST Failed");
      }
      httpClient.end();
    }
    // /* print*/
    // Serial.print(sensors::getTemp());
    // Serial.print(",");
    Serial.println(sensors::getHumid());
    // Serial.print(",");
    // Serial.print(sensors::getPressure());
    // Serial.print(",");
    // Serial.println(sensors::getCO2());
    vTaskDelay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  mic::setup();

  sensors::setup();
  xTaskCreatePinnedToCore(mainTask, "mainTask", 0x1000, NULL, 1, NULL, 1);
}

void loop() {
  delay(100);

  if (Serial.available() > 0) {
    char incoming = Serial.read();
    if (incoming == 'r') {
      ESP.restart();
    }
  }
}
