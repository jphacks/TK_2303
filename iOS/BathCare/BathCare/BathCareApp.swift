//
//  BathCareApp.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI
import FirebaseCore

@main
struct BathCareApp: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var delegate
    var body: some Scene {
        WindowGroup {
            ContentView(isOnboardingFinished: TokenManager.shared.hasToken)
        }
    }
}
