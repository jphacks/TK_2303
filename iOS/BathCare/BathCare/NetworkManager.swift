//
//  NetworkManager.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import Foundation
import SystemConfiguration.CaptiveNetwork
import CoreLocation

class NetworkManager {
    static let shared = NetworkManager()
    private var locationManager = CLLocationManager()
    func getNetwork() {
        print("getNetwork")
        let status = locationManager.authorizationStatus
        print("status:\(status.rawValue)")
        if status == .authorizedWhenInUse {
            if let interfaces = CNCopySupportedInterfaces() as NSArray? {
                for interface in interfaces {
                    if let interfaceInfo = CNCopyCurrentNetworkInfo(interface as! CFString) as NSDictionary?,
                        let ssid = interfaceInfo[kCNNetworkInfoKeySSID as String] as? String {
                        print(ssid)
                    }
                }
            }
        } else {
            locationManager.requestWhenInUseAuthorization()
        }
    }
}
