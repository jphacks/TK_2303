//
//  HomeViewModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import Foundation
class HomeViewModel: ObservableObject {
    let baseUrl = ""
    @Published var humidity: Double = 62.0
    @Published var temperature: Double = 25.0
    @Published var bathStatus: BathStatus = .inBath
    var bathStatusString: String {
        switch bathStatus {
        case .inBath:
            return "入浴中"
        case .outBath:
            return "空席"
        case .danger:
            return "危険"
        case .unknown:
            return "接続されていません"
        }
    }
    init() {
//        Task { @MainActor in
//            await makeRequest()
//        }
    }
    
    func makeRequest() async {
        let urlString = baseUrl + ""
        print("post url: ", urlString)
        let url = URL(string: urlString)!
        do {
            let (data,_) = try await URLSession.shared.data(from: url, delegate: nil)
            print("json got")
            let decoder = JSONDecoder()
            let status = try decoder.decode(BathStatus.self, from: data)
        } catch(let error) {
            print("error: \(error)")
        }
    }
}
