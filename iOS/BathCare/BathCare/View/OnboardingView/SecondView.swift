//
//  SecondView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/02.
//

import SwiftUI

struct SecondView: View {
    @Binding var currentPage: Int
    @Binding var state: OnboardingView.OnboardingState
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack(spacing: 16) {
                Image("Onboarding2")
                Text("What is ホッとお風呂?")
                    .font(.title2)
                    .bold()
                Text("浴室に設置することで、危険を事前に検知したり、浴室内での事故を素早くお知らせしたりすることができます")
                    .multilineTextAlignment(.center)
                Spacer()
            }
            .padding(.horizontal, 32)
            .padding(.vertical, 64)
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
    SecondView(
        currentPage: .constant(0),
        state: .constant(.stable)
    )
}
