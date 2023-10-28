//
//  HomeView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI

struct HomeView: View {
    var body: some View {
        ZStack {
            Color.backgroundColor
                .ignoresSafeArea()
            VStack(spacing: 0) {
                HStack {
                    Text("BathCare")
                        .font(.title)
                        .bold()
                        .foregroundStyle(Color.font)
                    Spacer()
                    Image(systemName: "bell.fill")
                        .resizable()
                        .frame(width: 24, height: 24)
                        .foregroundStyle(.font)
                }
                .padding(.horizontal, 24)
                .padding(.top, 24)
                .padding(.bottom, 8)
                .background(Color.white)
                Rectangle()
                    .foregroundStyle(.gray)
                    .frame(height: 1)
                ScrollView {
                    VStack(spacing: 16) {
                        Text("2023年10月28日")
                            .font(.headline)
                            .foregroundStyle(Color.gray)
                        Text("入浴中")
                            .font(.largeTitle)
                            .foregroundStyle(Color.font)
                            .bold()
                        HStack(spacing: 40) {
                            VStack {
                                Text("温度")
                                    .font(.body)
                                    .foregroundStyle(Color.black.opacity(0.6))
                                Text("45°")
                                    .font(.title2)
                                    .foregroundStyle(Color.black.opacity(0.7))
                            }
                            VStack {
                                Text("湿度")
                                    .font(.body)
                                    .foregroundStyle(Color.black.opacity(0.6))
                                Text("45%")
                                    .font(.title2)
                                    .foregroundStyle(Color.black.opacity(0.7))
                            }
                        }
                    }
                    .frame(maxWidth: .infinity)
                    .padding(24)
                    .background(Color.white)
                }
            }
        }
    }
}

#Preview {
    HomeView()
}
