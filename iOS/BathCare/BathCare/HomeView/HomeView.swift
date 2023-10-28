//
//  HomeView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI

struct HomeView: View {
    @ObservedObject var viewModel: HomeViewModel
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
                        Text(viewModel.bathStatusString)
                            .font(.largeTitle)
                            .foregroundStyle(viewModel.bathStatusColor)
                            .bold()
                        HStack(spacing: 40) {
                            VStack {
                                Text("温度")
                                    .font(.body)
                                    .foregroundStyle(Color.black.opacity(0.6))
                                Text(String(format: "%.1f", viewModel.temperature) + "°")
                                    .font(.title2)
                                    .foregroundStyle(Color.black.opacity(0.7))
                            }
                            VStack {
                                Text("湿度")
                                    .font(.body)
                                    .foregroundStyle(Color.black.opacity(0.6))
                                Text(String(format: "%.1f", viewModel.humidity) + "°")
                                    .font(.title2)
                                    .foregroundStyle(Color.black.opacity(0.7))
                            }
                        }
                    }
                    .frame(maxWidth: .infinity)
                    .padding(24)
                    .background(Color.white)
                }
                HeatShockPopupView(showingPopup: true)
            }
        }
    }
}

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
    HomeView(viewModel: .init())
}
