//
//  ThirdView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct ThirdView: View {
    @Binding var currentPage: Int
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack {
                Image("Onboarding3")
                Text("Start Setup")
                    .font(.title2)
                    .bold()
                Text("それではセットアップを始めましょう")
                    .padding(32)
                    .multilineTextAlignment(.center)
            }
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
    ThirdView(currentPage: .constant(0))
}
