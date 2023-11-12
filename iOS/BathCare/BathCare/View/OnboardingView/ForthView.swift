//
//  ForthView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct ForthView: View {
    @Binding var currentPage: Int
    @Binding var isOnboardingFinished: Bool
    @Binding var state: OnboardingView.OnboardingState
    @State var isConnected = false
    @State var isTokenExist: Bool? = nil
    @State var presentConnectingSheet = false
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack(spacing: 16) {
                Color.green.frame(
                    width: 280,
                    height: 280
                )
                Text("Connect to Device")
                    .font(.title2)
                    .bold()
                    
                Text(isConnected ? "デバイスと接続が完了しました。" :  "デバイスと接続を始めます。\n上の画像のボタンを長押ししてください。")
                    .multilineTextAlignment(.center)
                Spacer()
            }
            .padding(.horizontal, 32)
            .padding(.vertical, 64)
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            Button {
                if isTokenExist == true {
                    isOnboardingFinished = true
                } else if isConnected {
                    if state == .stable {
                        state = .animating
                        currentPage += 1
                    }
                } else {
                    presentConnectingSheet = true
                }
            } label: {
                Text(isConnected ? (isTokenExist ?? false ? "FINISH" : "NEXT") : "Connect")
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
        .sheet(isPresented: $presentConnectingSheet) {
            ConnectingBluetoothView(
                viewModel: .init(
                    bluetoothManager: .shared,
                    isConnected: $isConnected,
                    isTokenExist: $isTokenExist
                )
            )
            .presentationDetents([.medium])
        }
    }
}



#Preview {
    ForthView(
        currentPage: .constant(0),
        isOnboardingFinished: .constant(true),
        state: .constant(.stable)
    )
}
