//
//  HomeViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI
class HomeViewModel: ObservableObject {
    let baseUrl = "https://bathcare.tsubame.workers.dev/"
    @Published var humidity: Double = 62.0
    @Published var temperature: Double = 25.0
    @Published var bathStatus: BathStatus = .outBath
    @Published var isHighHeatShockPossiblity = false
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
    
    func makeRequest() async {
        let urlString = baseUrl + "status/9910"
        print("post url: ", urlString)
        let url = URL(string: urlString)!
        do {
            let (data,_) = try await URLSession.shared.data(from: url, delegate: nil)
            print("json got")
            let decoder = JSONDecoder()
            let response = try decoder.decode(BathStatusRequestJson.self, from: data)
            self.bathStatus = response.status
            self.isHighHeatShockPossiblity = true
        } catch(let error) {
            print("error: \(error)")
        }
    }
}
