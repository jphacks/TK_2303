//
//  BathStatusService.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/09.
//

import Foundation
class BathStatusService: Service {
    let id = 9910
    func fetch() async throws -> BathStatusJson {
        // URLとURLRequestの設定
        let url = URL(string: baseUrl + "/bath/status")!
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        request.setValue(bearerToken, forHTTPHeaderField: "Authorization")
        let (data, _) = try await URLSession.shared.data(for: request)
        return try JSONDecoder().decode(BathStatusJson.self, from: data)
    }
}
