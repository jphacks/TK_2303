//
//  TokenManager.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/12.
//

import Foundation
class TokenManager {
    static let shared = TokenManager()
    var hasToken: Bool {
        return token != nil
    }
    @Published private var token: String?
    
    init() {
        if let token = UserDefaults.standard.value(forKey: "userToken") as? String {
            self.token = token
        }
    }
    
    func registerToken() async {
        do {
            let registerDeviceJson = try await RegisterDeviceService().fetch()
            self.token = registerDeviceJson.token
            self.setToken()
        } catch(let error) {
            print(error)
        }
    }
    
    func getToken() -> String {
        guard let token else { fatalError() }
        return token
    }
    
    func setToken() {
        UserDefaults.standard.set(self.token, forKey: "userToken")
    }
    
    func deleteToken() {
        UserDefaults.standard.removeObject(forKey: "userToken")
    }
}
