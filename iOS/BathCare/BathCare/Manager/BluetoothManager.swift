//
//  BluetoothService.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import Foundation
import CoreBluetooth

protocol BluetoothManagerDelegate: AnyObject {
    func connected(token: String)
    func endConnecting()
    func gotNetworkAvailability(isNetworkAvailable: Bool)
//    func dataUpdated(rawData: RawData)
}

class BluetoothManager: NSObject {
    let kUARTServiceUUID = "2309e44f-cb8d-43fc-95b2-4c7134c23467" // サービス
    let kRXCharacteristicUUID = "37216a09-9f31-40f7-ab16-54ae5b32fd19" // ペリフェラルからの受信用

    var centralManager: CBCentralManager!
    var peripheral: CBPeripheral!
    var serviceUUID : CBUUID!
    
    var kRXCBCharacteristic: CBCharacteristic?
    var charcteristicUUIDs: [CBUUID]!
    
    weak var delegate: BluetoothManagerDelegate?
    
    static var shared: BluetoothManager = BluetoothManager()
    
    func setup(){
        print("setup...")

        centralManager = CBCentralManager()
        centralManager.delegate = self as CBCentralManagerDelegate

        serviceUUID = CBUUID(string: kUARTServiceUUID)
        charcteristicUUIDs = [CBUUID(string: kRXCharacteristicUUID)]
    }
    
    struct WifiData: Codable {
        var ssid: String
        var pass: String
    }
    
    func sendWifi(ssid: String, password: String){
        guard let kRXCBCharacteristic = kRXCBCharacteristic else {
            return
        }
        let wifidata = WifiData(ssid: ssid, pass: password)
        let encoder = JSONEncoder()
        if let writeData = try? encoder.encode(wifidata) {
            peripheral.writeValue(writeData, for: kRXCBCharacteristic, type: .withResponse)
        }
    }
    
    func sendEmptyData() {
        guard let kRXCBCharacteristic = kRXCBCharacteristic else {
            return
        }
        struct EmptyData: Codable {
            var getToken: Bool = true
        }
        let data = EmptyData()
        let encoder = JSONEncoder()
        if let writeData = try? encoder.encode(data) {
            peripheral.writeValue(writeData, for: kRXCBCharacteristic, type: .withResponse)
        }
    }
    
    func sendToken(token: String) {
        guard let kRXCBCharacteristic = kRXCBCharacteristic else {
            return
        }
        struct TokenData: Codable {
            var token: String
        }
        let data = TokenData(token: token)
        let encoder = JSONEncoder()
        if let writeData = try? encoder.encode(data) {
            peripheral.writeValue(writeData, for: kRXCBCharacteristic, type: .withResponse)
        }
    }
}

extension BluetoothManager: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        print("CentralManager didUpdateState")

        switch central.state {
            
        //電源ONを待って、スキャンする
        case CBManagerState.poweredOn:
            let services: [CBUUID] = [serviceUUID]
            centralManager?.scanForPeripherals(withServices: services,
                                               options: nil)
        default:
            break
        }
    }
    
    /// ペリフェラルを発見すると呼ばれる
    func centralManager(_ central: CBCentralManager,
                        didDiscover peripheral: CBPeripheral,
                        advertisementData: [String : Any],
                        rssi RSSI: NSNumber) {
        
        self.peripheral = peripheral
        centralManager?.stopScan()
        
        //接続開始
        central.connect(peripheral, options: nil)
        print("  - centralManager didDiscover")
    }
    
    /// 接続されると呼ばれる
    func centralManager(_ central: CBCentralManager,
                        didConnect peripheral: CBPeripheral) {
        
        peripheral.delegate = self
        peripheral.discoverServices([serviceUUID])
        print("  - centralManager didConnect")
    }
    
    /// 切断されると呼ばれる？
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print(#function)
        if error != nil {
            print(error.debugDescription)
            setup() // ペアリングのリトライ
            return
        }
    }
}

//MARK : - CBPeripheralDelegate
extension BluetoothManager: CBPeripheralDelegate {
    
    /// サービス発見時に呼ばれる
    func peripheral(_ peripheral: CBPeripheral,
                    didDiscoverServices error: Error?) {
        
        if error != nil {
            print(error.debugDescription)
            return
        }
        
        //キャリアクタリスティク探索開始
        if let service = peripheral.services?.first {
            print("Searching characteristic...")
            peripheral.discoverCharacteristics(charcteristicUUIDs,
                                               for: service)
        }
    }
    
    /// キャリアクタリスティク発見時に呼ばれる
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if error != nil {
            print(error.debugDescription)
            return
        }

        print("service.characteristics.count: \(service.characteristics!.count)")
        for characteristics in service.characteristics! {
            if(characteristics.uuid == CBUUID(string: kRXCharacteristicUUID)) {
                self.kRXCBCharacteristic = characteristics
                print("kTXCBCharacteristic did discovered!")
            }
        }
        
        if(self.kRXCBCharacteristic != nil) {
            startReciving()
        }
        print("  - Characteristic didDiscovered")
        
    }
    
    private func startReciving() {
        guard let kRXCBCharacteristic = kRXCBCharacteristic else {
            return
        }
        peripheral.setNotifyValue(true, for: kRXCBCharacteristic)
        print("Start monitoring the message from Arduino.\n\n")
        sendEmptyData()
    }

    /// データ送信時に呼ばれる
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
        print(#function)
        if error != nil {
            print(error.debugDescription)
            return
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor descriptor: CBDescriptor, error: Error?) {
        print(#function)
    }
    
    /// データ更新時に呼ばれる
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        print(#function)

        if error != nil {
            print(error.debugDescription)
            return
        }
        decodeTokenData(data: characteristic.value!)
    }
    
    func decodeTokenData(data: Data) {
        struct TokenJson: Codable {
            let token: String
        }
        struct NetworkAvailableJson: Codable {
            let isNetworkAvailable: Bool
        }
        let decoder = JSONDecoder()
        if let data = try? decoder.decode(TokenJson.self,from: data) {
            delegate?.connected(token: data.token)
        } else if let data = try? decoder.decode(NetworkAvailableJson.self, from: data) {
            print("hoge")
            delegate?.gotNetworkAvailability(isNetworkAvailable: data.isNetworkAvailable)
        } else {
            print("error", String(data: data, encoding: .utf8)!)
        }
    }
    
}
