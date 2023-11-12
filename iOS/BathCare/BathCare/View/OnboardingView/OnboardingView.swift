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
    var body: some View {
        PageViewController(pages: [
            AnyView(FirstView(currentPage: $currentPage)),
            AnyView(SecondView(currentPage: $currentPage)),
            AnyView(ThirdView(currentPage: $currentPage)),
            AnyView(ForthView(currentPage: $currentPage)),
            AnyView(FifthView(
                isOnboardingFinished: $isOnboardingFinished,
                viewModel: .init(bluetoothManager: .shared)
            )),
        ], currentPage: $currentPage)
    }
}

#Preview {
    OnboardingView(isOnboardingFinished: .constant(false))
}
