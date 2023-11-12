//
//  OnboardingView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/02.
//

import SwiftUI

struct OnboardingView: View {
    @State var currentPage: Int = 0
    var body: some View {
        PageViewController(pages: [
            AnyView(FirstView(currentPage: $currentPage)),
            AnyView(SecondView(currentPage: $currentPage)),
            AnyView(ThirdView(currentPage: $currentPage)),
            AnyView(ForthView(currentPage: $currentPage)),
            AnyView(FifthView(
                currentPage: $currentPage,
                viewModel: .init(bluetoothManager: .shared)
            )),
        ], currentPage: $currentPage)
    }
}

#Preview {
    OnboardingView()
}
