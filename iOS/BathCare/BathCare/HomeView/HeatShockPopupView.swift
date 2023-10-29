//
//  HeatShockPopupView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import SwiftUI

struct HeatShockPopupView: View {
    @State var showingPopup: Bool = true
    var body: some View {
        Text("気温が低下しています。\nヒートショックに気をつけてください")
            .foregroundStyle(Color.black.opacity(0.6))
            .multilineTextAlignment(.center)
            .padding(16)
            .background(Color.red.opacity(0.4))
            .cornerRadius(16)
            .padding(16)
            .opacity(showingPopup ? 1.0 : 0.0)
            .onAppear {
                withAnimation(.default.delay(2.0)) {
                    showingPopup = false
                }
            }
    }
}

#Preview {
    HeatShockPopupView()
}
