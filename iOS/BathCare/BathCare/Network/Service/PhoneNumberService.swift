//
//  PhoneNumberJson.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/09.
//

import Foundation
class PhoneNumberService: Service {
    let id = 9910
    func fetch() async throws -> PhoneNumberJson {
        // URLとURLRequestの設定
        let url = URL(string: baseUrl + "/phonenumber/" + String(id))!
        let (data, _) = try await URLSession.shared.data(from: url)
        return try JSONDecoder().decode(PhoneNumberJson.self, from: data)
    }
}
