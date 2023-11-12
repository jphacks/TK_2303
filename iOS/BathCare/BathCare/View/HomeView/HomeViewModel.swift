//
//  HomeViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI
import AudioToolbox
class HomeViewModel: ObservableObject {
    let baseUrl = "https://bathcare.tsubame.workers.dev"
    @Published var humidity: Double = 62.0
    @Published var temperature: Double = 25.0
    @Published var co2: Int = 0
    @Published var lastUpdate: String = ""
    @Published var bathStatus: BathStatus = .outBath
    @Published var isHighHeatShockPossiblity = false
    @Published var isAlertViewPresented = false
    @Published var notifications: [ActionNotification] = []
    private var phoneNumber: String?
    private var heatShockPopupShowed = false
    
    let id = 9910
    
    var timer: Timer?
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
        case .unknown:
            return "接続されていません"
        }
    }
    
    init() {
        Task { @MainActor in
            await makeRequest()
        }
    }
    
    func onAppear() {
        timer = Timer.scheduledTimer(withTimeInterval: 10, repeats: true) { _ in
            Task { @MainActor in
                await self.makeRequest()
            }
        }
        print("appeared: ", heatShockPopupShowed, isHighHeatShockPossiblity)
        if !heatShockPopupShowed {
            withAnimation(.default.delay(1.0)) {
                self.isHighHeatShockPossiblity = true
            }
            heatShockPopupShowed = true
        } else {
            isHighHeatShockPossiblity = false
        }
    }
    
    func onDisapear() {
        timer?.invalidate()
    }
    
    func makeCall() {
        guard let phoneNumber else { return }
        guard let url = URL(string: "tel://" + phoneNumber) else { return }
        UIApplication.shared.open(url)
    }
    
    func makeImpact() {
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate)
    }
    
    func makeRequest() async {
        await getStatus()
        await getSensorDatas()
        await getPhoneNumber()
        await getHistory()
    }
    
    func getStatus() async {
        do {
            let urlString = baseUrl + "/status/" + String(id)
            print("post url: ", urlString)
            let url = URL(string: urlString)!
            let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
            let decoder = JSONDecoder()
            let response = try decoder.decode(BathStatusJson.self, from: data)
            print(response)
            self.bathStatus = response.status
            if self.bathStatus == .danger {
                withAnimation(.default.delay(2.0)){
                    self.isAlertViewPresented = true
                }
            }
        } catch(let error) {
            print("error: \(error)")
        }
    }
    
    func getSensorDatas() async {
        do {
            let urlString = baseUrl + "/sensors/" + String(id)
            print("post url: ", urlString)
            let url = URL(string: urlString)!
            let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
            let decoder = JSONDecoder()
            let response = try decoder.decode(SensorDataJson.self, from: data)
            self.humidity = response.humidity
            self.temperature = response.temperature
            self.co2 = response.co2
            self.lastUpdate = response.lastUpdate
            print(response)
        } catch(let error) {
            print("error: \(error)")
        }
    }
    
    func getPhoneNumber() async {
        do {
            let urlString = baseUrl + "/phonenumber/" + String(id)
            print("post url: ", urlString)
            let url = URL(string: urlString)!
            let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
            let decoder = JSONDecoder()
            let response = try decoder.decode(PhoneNumberJson.self, from: data)
            self.phoneNumber = response.number
            print(response)
        } catch(let error) {
            print("error: \(error)")
        }
    }
    
    func getHistory() async {
        do {
            let urlString = baseUrl + "/history/" + String(id)
            print("post url: ", urlString)
            let url = URL(string: urlString)!
            let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
            let decoder = JSONDecoder()
            let response = try decoder.decode(HistoryJson.self, from: data)
            print(response)
            notifications = response.history
        } catch(let error) {
            print("error: \(error)")
        }
    }
}
