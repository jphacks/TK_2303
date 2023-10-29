//
//  HomeModel.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import Foundation
struct SensorDataJson: Decodable {
    var temperature: Double
    var humidity: Double
    var co2: Int
    var lastUpdate: String
}

struct BathStatusRequestJson: Decodable {
    var status: BathStatus
}

struct PhoneNumberJson: Decodable {
    var number: String
}

enum BathStatus: String, Decodable {
    case inBath = "in"
    case outBath = "out"
    case danger
    case unknown
}

struct HistoryJson: Decodable {
    var history: [ActionNotification]
}

struct ActionNotification : Decodable {
    var action: Action
    var date: String
    var dateAgo: String? {
        let dateFormatter = ISO8601DateFormatter()
        let d = dateFormatter.date(from: date)
        return d?.agoText()
    }
    enum Action: String, Decodable {
        case goIntoBath
        case goOutOfBath
        case becomeDanger
    }
}
