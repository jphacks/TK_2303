//
//  HomeModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import Foundation
struct BathData: Decodable {
    var temperature: Double
    var humidity: Double
    var status: BathStatus
}

struct BathStatusRequestJson: Decodable {
    var status: BathStatus
}

enum BathStatus: String, Decodable {
    case inBath = "in"
    case outBath = "out"
    case danger
    case unknown
}
