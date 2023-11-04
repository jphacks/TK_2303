//
//  ConnectingBluetoothViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI
class ConnectingBluetoothViewModel: ObservableObject {
    var bluetoothManager: BluetoothManager
    @Binding var isConnected: Bool
    
    init(bluetoothManager: BluetoothManager, isConnected: Binding<Bool>) {
        self.bluetoothManager = bluetoothManager
        self._isConnected = isConnected
    }
    
    func setup() {
        bluetoothManager.setup()
    }
}

extension ConnectingBluetoothViewModel: BluetoothManagerDelegate {
    func connected() {
        isConnected = true
    }
    
    func endConnecting() {
        print("endConnecting")
    }
}
