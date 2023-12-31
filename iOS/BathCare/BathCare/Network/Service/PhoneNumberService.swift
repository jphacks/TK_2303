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
        let url = URL(string: baseUrl + "/misc/phonenumber")!
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        request.setValue(bearerToken, forHTTPHeaderField: "Authorization")

        let (data, _) = try await URLSession.shared.data(for: request)
        return try JSONDecoder().decode(PhoneNumberJson.self, from: data)
    }
    
    func post(number: String) async throws {
        let data = PhoneNumberJson(number: number)
        let url = URL(string: baseUrl + "/misc/phonenumber")!
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue(bearerToken, forHTTPHeaderField: "Authorization")
        request.httpBody = try JSONEncoder().encode(data)
        
        let (_, _) = try await URLSession.shared.data(for: request)
    }
}
