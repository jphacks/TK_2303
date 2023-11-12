//
//  RegisterDeviceService.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/12.
//

import Foundation
class RegisterDeviceService: Service {
    let authValue = "Bearer Register-API-hotbath-wOMTAJVKJNIL7XIhzaZBsI2Jzyyd2QRnJj83u29PQdX0ZMtXqroDnlJAfUJh9O7n1XNzzkRZKw1r0MJbQ4Xf2UWTWXiwkSPnVdGorJ7B8PceskkpNwA1lZAF5Q7StIlz84tYm2wR6C55s7vxippFpkbYQ1XJ6lrpB2cInNx4sUWo4pvNVikYAmvrEiREwTpGP4wtJYvwJKoxpnRDiPXEoQALoXHtE99XgLycjxvJaHIbWwPUqPvlvx059h4jYudxZrtJD4YE72HNCJfPscnyIdmV7j8pcXUgQGhDPzo9u0LFawUQLTcMqW0mpaR0fYKQSehOHDASQv74oxtBOPRrrwtfV1ObIJIFs638ZnktCyxkVbrDloW581SdmDlm19uP5Ap0xlhy39hwjgmlE7qwwzKbySW25WvEwQHzuR1mqO6KECu2qcNHoYy0r4n59YpmvbZBSnzbH69hUGkhfbm1fuqtp2RBAdTa4a1MnM5C8ls"
    func fetch() async throws -> RegisterDeviceJson {
        // URLとURLRequestの設定
        let url = URL(string: baseUrl + "/device/register")!
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue(authValue, forHTTPHeaderField: "Authorization")

        let (data, _) = try await URLSession.shared.data(for: request)
        return try JSONDecoder().decode(RegisterDeviceJson.self, from: data)
    }
}
