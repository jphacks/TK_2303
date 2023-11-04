#include "ble.hpp"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>


#define SERVICE_UUID "2309e44f-cb8d-43fc-95b2-4c7134c23467"
#define CHARACTERISTIC_UUID "37216a09-9f31-40f7-ab16-54ae5b32fd19"

#define BT_NAME "HotBath"

namespace ble
{
static bool ble_initialized = false;
static BLECharacteristic* pCharacteristic = NULL;
static BLEServer* pServer = NULL;

class BleServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
        Serial.println("[*] BLE connected");
    };

    void onDisconnect(BLEServer* pServer)
    {
        Serial.println("[*] BLE disconnected");
    }
};

class BleCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic* _pCharacteristic)
    {
        std::string received_data = _pCharacteristic->getValue();

        if (received_data.length() > 0) {
            Serial.println(received_data.c_str());
        }
    }
};

static BleServerCallbacks ble_server_callbacks;
static BleCallbacks ble_callbacks;

void begin()
{
    Serial.println("### BLE BEGIN ###");
    if (ble_initialized) {
        Serial.println("already initialized");
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
    Serial.println("init end.");
    ble_initialized = true;
}

void stop()
{
    Serial.println("### BLE STOP ###");
    if (!ble_initialized) {
        Serial.println("not initialized");
        return;
    }
    BLEDevice::getAdvertising()->stop();
    ble_initialized = false;
    Serial.println("stopped.");
}

void send(etl::string_view data)
{
    if (!ble_initialized) {
        Serial.println("not initialized");
        return;
    }
    pCharacteristic->setValue(data.data());
    pCharacteristic->notify();
}
}  // namespace ble
