//
//  FifthViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI
class FifthViewModel: ObservableObject {
    enum Status {
        case inputting
        case confirming
        case confirmed
        case error
    }
    
    var bluetoothManager: BluetoothManager
    
    @Published var status: Status = .inputting
    @Published var ssidInput = ""
    @Published var passwordInput = ""
    
    init(bluetoothManager: BluetoothManager) {
        self.bluetoothManager = bluetoothManager
    }
    
    func sendData() {
        status = .confirming
        bluetoothManager.sendWifi(ssid: ssidInput, password: passwordInput)
    }
}

extension FifthViewModel: BluetoothManagerDelegate {
    func connected(token: String?) {}
    func endConnecting() {}
}
