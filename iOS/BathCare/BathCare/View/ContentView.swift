//
//  ContentView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI

struct ContentView: View {
    @State var isOnboardingFinished: Bool
    var body: some View {
        NavigationStack {
            if isOnboardingFinished {
                HomeView(viewModel: .init(appDataStore: .shared))
            } else {
                OnboardingView(isOnboardingFinished: $isOnboardingFinished)
            }
        }
    }
}

#Preview {
    ContentView(isOnboardingFinished: true)
}
