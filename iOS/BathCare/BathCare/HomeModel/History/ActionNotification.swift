//
//  ActionNotification.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import Foundation
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
