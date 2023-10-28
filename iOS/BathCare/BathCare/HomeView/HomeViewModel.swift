//
//  HomeViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI
class HomeViewModel: ObservableObject {
    let baseUrl = "https://bathcare.tsubame.workers.dev"
    @Published var humidity: Double = 62.0
    @Published var temperature: Double = 25.0
    @Published var co2: Int = 0
    @Published var lastUpdate: String = ""
    @Published var bathStatus: BathStatus = .outBath
    @Published var isHighHeatShockPossiblity = false
    @Published var isAlertViewPresented = false
    private var phoneNumber: String?
    
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
    }
    
    func onDisapear() {
        timer?.invalidate()
    }
    
    func makeCall() {
        guard let phoneNumber else { return }
        guard let url = URL(string: "tel://" + phoneNumber) else { return }
        UIApplication.shared.open(url)
    }
    
    func makeRequest() async {
        do {
            try await getStatus()
            try await getSensorDatas()
            try await getPhoneNumber()
        } catch(let error) {
            print("error: \(error)")
        }
    }
    
    func getStatus() async throws {
        let urlString = baseUrl + "/status/" + String(id)
        print("post url: ", urlString)
        let url = URL(string: urlString)!
        let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
        let decoder = JSONDecoder()
        let response = try decoder.decode(BathStatusRequestJson.self, from: data)
        print(response)
        self.bathStatus = response.status
        if self.bathStatus == .danger {
            withAnimation(.default.delay(2.0)){
                self.isAlertViewPresented = true
            }
        }
        withAnimation {
            self.isHighHeatShockPossiblity = true
        }
    }
    
    func getSensorDatas() async throws {
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
    }
    
    func getPhoneNumber() async throws {
        let urlString = baseUrl + "/phonenumber/" + String(id)
        print("post url: ", urlString)
        let url = URL(string: urlString)!
        let (data, _) = try await URLSession.shared.data(from: url, delegate: nil)
        let decoder = JSONDecoder()
        let response = try decoder.decode(PhoneNumberJson.self, from: data)
        self.phoneNumber = response.number
        print(response)
    }
}
