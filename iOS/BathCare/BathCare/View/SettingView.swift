//
//  SettingView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/12.
//

import SwiftUI

struct SettingView: View {
    @Environment(\.dismiss) private var dismiss
    @State var numberText: String
    var body: some View {
        VStack(spacing: 15) {
            VStack(spacing: 0) {
        #if DEBUG
                HStack {
                    Text("ログアウト")
                        .frame(width: 80)
                    Text(":")
                    Button("ログアウト") {
                        TokenManager.shared.deleteToken()
                        exit(0)
                    }
                    Spacer()
                }
                .bold()
                .padding(.horizontal, 12)
                .padding(.vertical, 16)
                Rectangle()
                    .foregroundStyle(.black.opacity(0.15))
                    .frame(height: 1)
                HStack {
                    Text("mytoken")
                        .frame(width: 80)
                    Text(":")
                    Text(TokenManager.shared.getToken())
                    Spacer()
                }
                .bold()
                .padding(.horizontal, 12)
                .padding(.vertical, 16)
                Rectangle()
                    .foregroundStyle(.black.opacity(0.15))
                    .frame(height: 1)
        #endif
                HStack {
                    Text("緊急連絡先")
                        .frame(width: 80)
                    Text(":")
                    TextField("入力してください", text: $numberText)
                        .textFieldStyle(.plain)
                    Spacer()
                }
                .bold()
                .padding(.horizontal, 12)
                .padding(.vertical, 16)
                Rectangle()
                    .foregroundStyle(.black.opacity(0.15))
                    .frame(height: 1)
            }
            .background(Color.backGround)
            .cornerRadius(12)
            .padding(16)
            Button {
                Task {
                    do {
                        try await PhoneNumberService().post(number: numberText)
                    } catch(let error){
                        print(error)
                    }
                    dismiss()
                }
            } label: {
                Text("変更する")
                    .bold()
                    .font(.title)
                    .foregroundStyle(.white)
                    .frame(maxWidth: .infinity)
                    .padding(.vertical, 16)
                    .background(Color.font)
                    .cornerRadius(32)
                    .padding(.horizontal, 48)
            }
        }.navigationTitle("Settings")
    }
}

#Preview {
    SettingView(numberText: "")
}
