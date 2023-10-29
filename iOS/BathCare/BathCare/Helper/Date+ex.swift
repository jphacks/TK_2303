//
//  Date+ex.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import Foundation
extension Date {
    static let dateFormatter: DateFormatter = {
        return DateFormatter()
    }()

    func dateText() -> String {
        Date.dateFormatter.dateFormat = "yyyy/MM/dd(E)"
        return Date.dateFormatter.string(from: self)
    }

    func agoText() -> String {
        let now = Date()
        let (earliest, latest) = self < now ? (self, now) : (now, self)
        let components = Calendar.current.dateComponents([.year, .month, .weekOfYear, .day, .hour, .minute, .second],
                                                         from: earliest, to: latest)
        if let year = components.year, year > 0 {
            return self.dateText()
        }
        if let month = components.month, month > 0 {
            return self.dateText()
        }
        if let weekOfYear = components.weekOfYear, weekOfYear > 0 {
            return self.dateText()
        }
        if let day = components.day, day > 0 {
            return "\(day)日前"
        }
        if let hour = components.hour, hour > 0 {
            return "\(hour)時間前"
        }
        if let minute = components.minute, minute > 0 {
            return "\(minute)分前"
        }
        if let second = components.second, second > 0 {
            return "\(second)秒前"
        }
        return "たった今"
    }
} 
