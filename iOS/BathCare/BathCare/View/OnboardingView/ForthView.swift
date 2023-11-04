//
//  ForthView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/04.
//

import SwiftUI

struct ForthView: View {
    @Binding var currentPage: Int
    @State var connected = false
    var bluetoothManager: BluetoothManager
    var body: some View {
        ZStack(alignment: .bottom) {
            VStack {
                Color.green.frame(
                    width: 280,
                    height: 280
                )
                Text("Connect to Device")
                    .font(.title2)
                    .bold()
                    
                Text("デバイスと接続を始めます。\n上の画像のボタンを長押ししてください。")
                    .padding(32)
                    .multilineTextAlignment(.center)
            }
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            Button {
                if connected {
                    currentPage += 1
                } else {
                    
                }
            } label: {
                Text(connected ? "NEXT" : "Connect")
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
        .onAppear {
            bluetoothManager.setup()
        }
    }
}

#Preview {
    ForthView(
        currentPage: .constant(0),
        bluetoothManager: BluetoothManager.shared
    )
}
