//
//  HomeViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI
import AudioToolbox
import Combine

class HomeViewModel: ObservableObject {
//    @Published var isHighHeatShockPossiblity = false
    @Published var isAlertViewPresented = false
    @Published var notifications: [ActionNotification] = []
    private var heatShockPopupShowed = false
    
    private var appDataStore: AppDataStore
    private var cancellables = Set<AnyCancellable>()
    
    let id = 9910
    
    var timer: Timer?
    @Published var bathStatus: BathStatus?
    @Published var temperature: Double?
    @Published var humidity: Double?
    @Published var phoneNumber: String?
    
    var bathStatusColor: Color {
        switch bathStatus {
        case .inBath:
            Color.fontColor
        case .danger:
            Color.red
        default:
            Color.gray
        }
    }
    
    var bathStatusString: String {
        switch bathStatus {
        case .inBath:
            return "入浴中"
        case .outBath:
            return "入浴していません"
        case .danger:
            return "危険"
        default:
            return "接続されていません"
        }
    }
    
    init(appDataStore: AppDataStore) {
        self.appDataStore = appDataStore
        appDataStore.$storedData
            .sink { [weak self] newData in
                self?.bathStatus = newData.bathStatusJson?.status
                self?.humidity = newData.sensorDatasJson?.humidity
                self?.temperature = newData.sensorDatasJson?.temperature
                self?.phoneNumber = newData.phoneNumberJson?.number
                self?.notifications = newData.historyJson?.history ?? []
            }
            .store(in: &cancellables)
    }

    func onAppear() {
        timer = Timer.scheduledTimer(withTimeInterval: 60, repeats: true) { _ in
            Task { @MainActor in
                await self.refresh()
            }
        }
//        if !heatShockPopupShowed {
//            withAnimation(.default.delay(1.0)) {
//                self.isHighHeatShockPossiblity = true
//            }
//            heatShockPopupShowed = true
//        } else {
//            isHighHeatShockPossiblity = false
//        }
    }
    
    func onDisapear() {
        timer?.invalidate()
    }
    
    func refresh() async {
        let oldBathStatus = bathStatus
        await appDataStore.refresh()
        if bathStatus == .danger && oldBathStatus != .danger {
            withAnimation(.default.delay(2.0)) {
                isAlertViewPresented = true
            }
        }
    }
    
    func makeCall() {
        guard let phoneNumber else { return }
        guard let url = URL(string: "tel://" + phoneNumber) else { return }
        UIApplication.shared.open(url)
    }
    
    func makeImpact() {
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate)
    }
}
