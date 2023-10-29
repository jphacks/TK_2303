//
//  SensorDataJson.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import Foundation
struct SensorDataJson: Decodable {
    var temperature: Double
    var humidity: Double
    var co2: Int
    var lastUpdate: String
}
