//
//  ConnectingBluetoothView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct ConnectingBluetoothView: View {
    @Environment(\.dismiss) private var dismiss
    let timer = Timer.publish(every: 0.1, on: .main, in: .common).autoconnect()
    @State var value = 0.0
    @State var checkClippedStatus = -80
    
    @ObservedObject var viewModel: ConnectingBluetoothViewModel
    var body: some View {
        VStack(spacing: 24) {
            Button("debug"){
                viewModel.connected()
            }
            Text("デバイスを探しています")
                .font(.title2)
                .bold()
            ZStack {
                ZStack {
                    Image("Bluetooth")
                        .resizable()
                        .frame(width: 80, height: 80)
                    Color.white.opacity(0.4)
                        .frame(width: 80, height: 80)
                }
                .opacity(!viewModel.isConnected ? 1.0 : 0.0)
                Image(systemName: "slowmo", variableValue: value)
                    .resizable()
                    .frame(width: 200, height: 200)
                    .foregroundStyle(.black.opacity(0.5))
                    .opacity(!viewModel.isConnected ? 1.0 : 0.0)
                Image(systemName: "checkmark")
                    .resizable()
                    .frame(width: 80, height: 80)
                    .foregroundStyle(.font)
                    .clipShape(Rectangle().offset(x: CGFloat(checkClippedStatus)))
            }
            HStack {
                ZStack {
                    Image(systemName: "iphone")
                        .resizable()
                        .frame(width: 40, height: 64)
                    Color.white.opacity(0.4)
                        .frame(width: 64, height: 80)
                }
                ZStack {
                    Image(systemName: "arrow.right")
                        .resizable()
                        .frame(width: 40, height: 40)
                        .clipShape(
                            Rectangle()
                                .offset(x: viewModel.isConnected ? 0 :  -40 + value * 40)
                        )
                    Color.white.opacity(0.4)
                        .frame(width: 64, height: 80)
                }
                ZStack {
                    Image(systemName: "cube")
                        .resizable()
                        .frame(width: 56, height: 64)
                    Color.white.opacity(0.4)
                        .frame(width: 64, height: 80)
                }
            }
        }
        .onAppear {
            viewModel.setup()
        }
        .onChange(of: viewModel.isConnected) {
            if viewModel.isConnected {
                withAnimation {
                    checkClippedStatus = 0
                    Task { @MainActor in
                        try await Task.sleep(for: .seconds(1.5))
                        dismiss()
                    }
                }
            }
        }
        .onReceive(timer) { _ in
            if value < 0.98 {
                value += 0.1
            } else {
                value = 0.0
            }
        }
    }
}

#Preview {
    ConnectingBluetoothView(
        viewModel: .init(
            bluetoothManager: .shared,
            isConnected: .constant(false)
        )
    )
}
