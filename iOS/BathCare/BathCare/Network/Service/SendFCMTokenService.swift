//
//  SendFCMTokenService.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/17.
//

import Foundation
class SendFCMTokenService: Service {
    func post(token: String) async throws {
        struct PostData: Codable {
            let token: String
        }
        let data = PostData(token: token)
        // URLとURLRequestの設定
        let url = URL(string: baseUrl + "/misc/alart/token")!
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        if !TokenManager.shared.hasToken {
            return
        }
        request.setValue(bearerToken, forHTTPHeaderField: "Authorization")
        request.httpBody = try JSONEncoder().encode(data)

        let (_, _) = try await URLSession.shared.data(for: request)
    }
}
