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
    @Binding var isTokenExist: Bool?
    
    init(bluetoothManager: BluetoothManager, isConnected: Binding<Bool>, isTokenExist: Binding<Bool?>) {
        self.bluetoothManager = bluetoothManager
        self._isConnected = isConnected
        self._isTokenExist = isTokenExist
        self.bluetoothManager.delegate = self
    }
    
    func setup() {
        bluetoothManager.setup()
    }
}

extension ConnectingBluetoothViewModel: BluetoothManagerDelegate {
    func connected(token: String) {
        if token != "" {
            isTokenExist = true
            TokenManager.shared.setToken(token: token)
        } else {
            isTokenExist = false
        }
        isConnected = true
    }
    func endConnecting() {}
    func gotNetworkAvailability(isNetworkAvailable: Bool) {}
}
