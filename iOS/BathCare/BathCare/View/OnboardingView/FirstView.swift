//
//  FirstView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/02.
//

import SwiftUI

struct FirstView: View {
    @Binding var currentPage: Int
    var bounds = UIScreen.main.bounds
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack(spacing: 16) {
                Image("Onboarding1")
                Text("Welcome to ホッとお風呂")
                    .font(.title2)
                    .bold()
                Text("ホッとお風呂は家族に安心を届けるデバイスです")
                    .multilineTextAlignment(.center)
                Spacer()
            }
            .padding(.horizontal, 32)
            .padding(.vertical, 64)
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            Button {
                currentPage += 1
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
    }
}

#Preview {
    FirstView(currentPage: .constant(0))
}
