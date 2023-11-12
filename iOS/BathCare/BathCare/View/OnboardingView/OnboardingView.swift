//
//  OnboardingView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/02.
//

import SwiftUI

struct OnboardingView: View {
    @State var currentPage: Int = 0
    @Binding var isOnboardingFinished: Bool
    @State var state: OnboardingState = .stable
    enum OnboardingState {
        case animating
        case stable
    }
    var body: some View {
        PageViewController(
            pages: [
                AnyView(
                    FirstView(
                        currentPage: $currentPage,
                        state: $state
                    )
                ),
                AnyView(
                    SecondView(
                        currentPage: $currentPage,
                        state: $state
                    )
                ),
                AnyView(
                    ThirdView(
                        currentPage: $currentPage,
                        state: $state
                    )
                ),
                AnyView(
                    ForthView(
                        currentPage: $currentPage,
                        isOnboardingFinished: $isOnboardingFinished,
                        state: $state
                    )
                ),
                AnyView(
                    FifthView(
                        isOnboardingFinished: $isOnboardingFinished,
                        state: $state,
                        viewModel: .init(bluetoothManager: .shared)
                    )
                ),
            ],
            currentPage: $currentPage,
            state: $state
        )
    }
}

#Preview {
    OnboardingView(isOnboardingFinished: .constant(false))
}
