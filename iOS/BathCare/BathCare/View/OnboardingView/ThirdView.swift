//
//  ThirdView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct ThirdView: View {
    @Binding var currentPage: Int
    @Binding var state: OnboardingView.OnboardingState
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack(spacing: 16) {
                Image("Onboarding3")
                Text("Start Setup")
                    .font(.title2)
                    .bold()
                Text("それではセットアップを始めましょう")
                    .multilineTextAlignment(.center)
                Spacer()
            }
            .padding(.vertical, 64)
            .padding(.horizontal, 32)
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            Button {
                if state == .stable {
                    state = .animating
                    currentPage += 1
                }
            } label: {
                Text("NEXT")
                    .bold()
                    .font(.title)
                    .foregroundStyle(.white)
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 16)
                    .background(Color.font)
                    .cornerRadius(32)
                    .padding(.horizontal, 48)
            }
        }
        .onDisappear{
            state = .stable
        }
    }
}

#Preview {
    ThirdView(
        currentPage: .constant(0),
        state: .constant(.stable)
    )
}
