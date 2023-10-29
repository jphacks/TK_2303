//
//  BathStatus.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import Foundation
enum BathStatus: String, Decodable {
    case inBath = "in"
    case outBath = "out"
    case danger
    case unknown
}
