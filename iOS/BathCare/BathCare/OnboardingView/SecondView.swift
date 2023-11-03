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
        Text("SecondView")
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
                .cornerRadius(16)
                .padding(.horizontal, 32)
        }
    }
}

#Preview {
    SecondView(currentPage: .constant(0))
}
