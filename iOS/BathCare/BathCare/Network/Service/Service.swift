//
//  Service.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/09.
//

import Foundation
class Service {
    public let baseUrl = "https://bathcare-test.tsubame.workers.dev"
    public var bearerToken: String {
        "Bearer " + TokenManager.shared.getToken()
    }
}
