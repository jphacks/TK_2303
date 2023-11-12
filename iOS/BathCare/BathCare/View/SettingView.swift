//
//  SettingView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/12.
//

import SwiftUI

struct SettingView: View {
    var body: some View {
        List {
            #if DEBUG
            Button("ログアウト") {
                TokenManager.shared.deleteToken()
                exit(0)
            }
            #endif
        }
    }
}

#Preview {
    SettingView()
}
