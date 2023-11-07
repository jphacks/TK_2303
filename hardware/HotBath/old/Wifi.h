#pragma once
#include <Arduino.h>
#include <WiFi.h>

namespace wifi {

bool status = false;

void setup(const char* ssid, const char* pass) {
  if ((WiFi.status() == WL_CONNECTED)) {
    return;
  }
  // WiFiのアクセスポイントに接続
  Serial.println("### WIFI INIT ###");
  Serial.print("WiFi connecting to ");
  Serial.print(ssid);
  Serial.print(" ");
  Serial.println(pass);
  WiFi.begin(ssid, pass);
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

//    void checkStatus() {
//      if ((WiFi.status() != WL_CONNECTED)) {
//        Serial.println("Reconnecting to WiFi...");
//        WiFi.disconnect();
//        WiFi.reconnect();
//      }
//    }

bool getStatus() {
  return status;
}
}