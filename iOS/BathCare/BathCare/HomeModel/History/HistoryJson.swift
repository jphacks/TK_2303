//
//  HistoryJson.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import Foundation
struct HistoryJson: Decodable {
    var history: [ActionNotification]
}
