//
//  FifthView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct FifthView: View {
    let timer = Timer.publish(every: 0.1, on: .main, in: .common).autoconnect()
    @State var value = 0.0
    @Binding var isOnboardingFinished: Bool
    @Binding var state: OnboardingView.OnboardingState
    @ObservedObject var viewModel: FifthViewModel
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack(spacing: 16) {
                VStack(alignment: .trailing) {
                    ZStack {
                        Image(systemName: "network")
                            .resizable()
                            .frame(width: 56, height: 56)
                            .foregroundStyle(Color.font)
                        Color.white.opacity(0.00)
                            .frame(width: 64, height: 80)
                    }
                    ZStack {
                        Image(systemName: "arrow.up")
                            .resizable()
                            .frame(width: 40, height: 40)
                            .foregroundStyle(
                                viewModel.status == .confirming || viewModel.status == .confirmed ? Color.font : Color.black.opacity(0.1)
                            )
                            .clipShape(
                                Rectangle().offset(y: viewModel.status == .confirming ? 40 - value * 40 : 0)
                            )
                        Color.white.opacity(0.00)
                            .frame(width: 64, height: 60)
                    }
                    HStack {
                        ZStack {
                            Image(systemName: "iphone")
                                .resizable()
                                .frame(width: 40, height: 64)
                                .foregroundStyle(Color.font)
                            Color.white.opacity(0.00)
                                .frame(width: 64, height: 80)
                        }
                        ZStack {
                            Image(systemName: "arrow.right")
                                .resizable()
                                .frame(width: 40, height: 40)
                                .foregroundStyle(Color.font)
                            Color.white.opacity(0.00)
                                .frame(width: 64, height: 80)
                        }
                        ZStack {
                            Image(systemName: "cube")
                                .resizable()
                                .frame(width: 56, height: 64)
                                .foregroundStyle(Color.font)
                            Color.white.opacity(0.00)
                                .frame(width: 64, height: 80)
                        }
                    }
                }
                .frame(width: 280, height: 280)
                Text("Connecting to Internet")
                    .font(.title2)
                    .bold()
                Text("Wifiを設定し、インターネットに接続します")
                    .multilineTextAlignment(.center)
                VStack(spacing: 0) {
                    HStack {
                        Text("SSID")
                            .frame(width: 80)
                        Text(":")
                        TextField("SSIDを入力", text: $viewModel.ssidInput)
                    }
                    .bold()
                    .padding(.horizontal, 8)
                    .padding(.vertical, 16)
                    Rectangle()
                        .foregroundStyle(.black.opacity(0.15))
                        .frame(height: 1)
                    HStack {
                        Text("Password")
                            .frame(width: 80)
                        Text(":")
                        SecureField("Passwordを入力", text: $viewModel.passwordInput)
                            .textFieldStyle(.plain)
                    }
                    .bold()
                    .padding(.horizontal, 12)
                    .padding(.vertical, 16)
                }
                .background(Color.backGround)
                .cornerRadius(12)
                #if DEBUG
//                Button("debug") {
//                    viewModel.gotNetworkAvailability(isNetworkAvailable: true)
//                }
                #endif
                Spacer()
            }
            .padding(.horizontal, 32)
            .padding(.vertical, 64)
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            Button {
                if state == .stable {
                    if viewModel.status == .confirmed {
                        isOnboardingFinished = true
                    } else {
                        viewModel.sendData()
                    }
                }
            } label: {
                Text(viewModel.status == .confirmed ? "FINISH" : "CONNECT")
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
        .onReceive(timer) { _ in
            if value < 0.98 {
                value += 0.1
            } else {
                value = 0.0
            }
        }
        .onAppear {
            viewModel.onAppear()
            viewModel.gotNetworkAvailability(isNetworkAvailable: true)
        }
        .alert(
            "接続に失敗しました。",
            isPresented: $viewModel.isAlertPresented,
            actions: {}
        )
    }
}

#Preview {
    struct PreviewWrapper: View {
        let viewModel: FifthViewModel = .init(bluetoothManager: .shared)
        var body: some View {
            viewModel.ssidInput = "TP-Link3308"
            viewModel.passwordInput = "TP-Link3308"
            return FifthView(
                isOnboardingFinished: .constant(false),
                state: .constant(.stable),
                viewModel: viewModel
            )
        }
    }
    return PreviewWrapper()
}
