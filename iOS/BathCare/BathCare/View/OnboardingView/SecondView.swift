//
//  SecondView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/02.
//

import SwiftUI

struct SecondView: View {
    @Binding var currentPage: Int
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack {
                Image("Onboarding2")
                Text("What is ホッとお風呂?")
                    .font(.title2)
                    .bold()
                Text("浴室に設置することで、危険を事前に検知したり、浴室内での事故を素早くお知らせしたりすることができます")
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
    SecondView(currentPage: .constant(0))
}
