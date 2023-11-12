//
//  HistoryService.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/09.
//

import Foundation
class HistoryService: Service {
    let id = 9910
    func fetch() async throws -> HistoryJson {
        // URLとURLRequestの設定
        let url = URL(string: baseUrl + "/history/" + String(id))!
        let (data, _) = try await URLSession.shared.data(from: url)
        return try JSONDecoder().decode(HistoryJson.self, from: data)
    }
}
