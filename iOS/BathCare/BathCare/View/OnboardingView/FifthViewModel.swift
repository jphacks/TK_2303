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
    @Published var isAlertPresented = false
    
    init(bluetoothManager: BluetoothManager) {
        self.bluetoothManager = bluetoothManager
    }
    
    func onAppear() {
        bluetoothManager.delegate = self
    }
    
    func sendData() {
        status = .confirming
        bluetoothManager.sendWifi(ssid: ssidInput, password: passwordInput)
    }
}

extension FifthViewModel: BluetoothManagerDelegate {
    func gotNetworkAvailability(isNetworkAvailable: Bool) {
        if isNetworkAvailable {
            Task { @MainActor in
                await TokenManager.shared.registerToken()
                let token = TokenManager.shared.getToken()
                bluetoothManager.sendToken(token: token)
                status = .confirmed
            }
        } else {
            isAlertPresented = true
            print("hello")
        }
    }
    
    func connected(token: String) {}
    func endConnecting() {}
}
