#include "ble.hpp"
#include "config.hpp"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "esp_log.h"
#include "led.hpp"
#include "utils.hpp"
#include "wifi.hpp"
#include <ArduinoJson.h>

#define SERVICE_UUID "2309e44f-cb8d-43fc-95b2-4c7134c23467"
#define CHARACTERISTIC_UUID "37216a09-9f31-40f7-ab16-54ae5b32fd19"

#define BT_NAME "HotBath"

#define BLE_TIMEOUT 1000 * 60 * 3
namespace ble
{
typedef enum {
    Normal = 0,
    OpenRequest,
    CloseRequest,
} BLE_Status;

static bool ble_opened = false;
static BLECharacteristic* pCharacteristic = NULL;
static BLEServer* pServer = NULL;
static char json_buf[256];
static StaticJsonDocument<100> doc;
static volatile BLE_Status status = Normal;
static int64_t last_operation_tick = 0;
static const char TAG[] = "BLE";
static volatile bool wifi_connect_request = false;
static volatile bool restart_request = false;

static void ble_task(void* pvParameters);
static void send(etl::string_view data);

class BleServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
        ESP_LOGI(TAG, "connected");
    }
    void onDisconnect(BLEServer* pServer)
    {
        ESP_LOGI(TAG, "disconnected");
    }
};

class BleCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* _pCharacteristic)
    {
        std::string received_data = _pCharacteristic->getValue();
        if (received_data.length() > 0) {
            last_operation_tick = get_tick();
            ESP_LOGI(TAG, "receive: %s", received_data.c_str());
            deserializeJson(doc, received_data);

            if (doc["getToken"] != nullptr) {
                doc.clear();
                if (config::data.token_configured) {
                    doc["token"] = config::data.token;
                    serializeJson(doc, json_buf);
                    send(json_buf);
                    restart_request = true;
                } else {
                    doc["token"] = "";
                    serializeJson(doc, json_buf);
                    send(json_buf);
                }
            } else if (doc["ssid"] != nullptr && doc["pass"] != nullptr) {
                snprintf(config::data.ssid, sizeof(config::data.ssid), "%s", doc["ssid"].as<const char*>());
                snprintf(config::data.pass, sizeof(config::data.pass), "%s", doc["pass"].as<const char*>());
                wifi_connect_request = true;
                config::save();
            } else if (doc["token"] != nullptr) {
                snprintf(config::data.token, sizeof(config::data.token), "%s", doc["token"].as<const char*>());
                config::data.token_configured = true;
                config::save();
            } else {
                ESP_LOGE(TAG, "Unknown json format");
            }
        }
    }
};

static BleServerCallbacks ble_server_callbacks;
static BleCallbacks ble_callbacks;

void init()
{
    if (config::data.token_configured == false) {
        status = OpenRequest;
    }
    xTaskCreatePinnedToCore(ble_task, "ble_task", 0x1000, NULL, 1, NULL, 1);
}

void open()
{
    ESP_LOGI(TAG, "open start");
    if (ble_opened) {
        ESP_LOGE(TAG, "already opened");
        status = Normal;
        return;
    }
    BLEDevice::init(BT_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(&ble_server_callbacks);
    BLEService* pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);
    pCharacteristic->setCallbacks(&ble_callbacks);
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    ESP_LOGI(TAG, "open completed");
    led::set_mode(led::Pairing);
    ble_opened = true;
    status = Normal;
}

void close()
{
    led::set_mode(led::Normal);
    ESP_LOGI(TAG, "close start");
    if (!ble_opened) {
        ESP_LOGE(TAG, "not opened");
        status = Normal;
        return;
    }
    BLEDevice::getAdvertising()->stop();
    ble_opened = false;
    status = Normal;
    ESP_LOGI(TAG, "closed");
}

void send(etl::string_view data)
{
    if (!ble_opened) {
        ESP_LOGE(TAG, "not opened");
        return;
    }
    pCharacteristic->setValue(data.data());
    pCharacteristic->notify();
    ESP_LOGI(TAG, "send: %s", data.data());
}

void open_request()
{
    status = OpenRequest;
}

void close_request()
{
    ESP.restart();
    // status = CloseRequest;
}

void ble_task(void* pvParameters)
{
    while (true) {
        if (status == OpenRequest) {
            open();
            last_operation_tick = get_tick();
        } else if (status == CloseRequest) {
            close();
        }
        if (ble_opened && get_tick() - last_operation_tick > BLE_TIMEOUT) {
            close();
        }
        if (wifi_connect_request) {
            config::data.wifi_configured = true;
            wifi::update();
            doc.clear();
            if (wifi::get_status()) {
                doc["isNetworkAvailable"] = true;
            } else {
                config::data.wifi_configured = false;
                doc["isNetworkAvailable"] = false;
            }
            serializeJson(doc, json_buf);
            send(json_buf);
            config::save();
            wifi_connect_request = false;
        }
        if (restart_request) {
            vTaskDelay(2000);
            ESP.restart();
        }
        vTaskDelay(1000);
    }
}
}  // namespace ble