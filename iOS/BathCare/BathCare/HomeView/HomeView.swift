//
//  HomeView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/28.
//

import SwiftUI

struct HomeView: View {
    @ObservedObject var viewModel: HomeViewModel
    @Namespace private var namespace
    var body: some View {
        if viewModel.isAlertViewPresented {
            HomeAlertView(
                namespace: namespace,
                showingAlertView: $viewModel.isAlertViewPresented,
                makeCall: viewModel.makeCall
            )
        } else {
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
                        Button {
                            // TODO: 消す
                            withAnimation(.default) {
                                viewModel.isAlertViewPresented = true
                                print("hello")
                            }
                        } label: {
                            Image(systemName: "bell.fill")
                                .resizable()
                                .matchedGeometryEffect(id: "bell", in: namespace)
                                .frame(width: 24, height: 24)
                                .foregroundStyle(.font)
                        }
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
                    .refreshable {
                        Task { @MainActor in
                            await viewModel.makeRequest()
                        }
                    }
                    HeatShockPopupView(showingPopup: true)
                }
            }
            .onAppear {
                viewModel.onAppear()
            }
            .onDisappear {
                viewModel.onDisapear()
            }
        }
        
    }
}

struct HomeAlertView: View {
    var namespace: Namespace.ID
    @Binding var showingAlertView: Bool
    var makeCall: () -> ()
    var body: some View {
        ZStack(alignment: .topTrailing) {
            Color.red.ignoresSafeArea()
            Button {
                withAnimation {
                    showingAlertView = false
                }
            } label: {
                Image(systemName: "x.circle")
                    .resizable()
                    .frame(width: 32, height: 32)
                    .padding(24)
                    .foregroundStyle(Color.white)
            }
            ZStack(alignment: .bottom){
                VStack {
                    Text("危険")
                        .font(.largeTitle)
                        .foregroundStyle(.white)
                        .padding(.bottom, 48)
                        .bold()
                    Image(systemName: "bell.fill")
                        .resizable()
                        .matchedGeometryEffect(id: "bell", in: namespace)
                        .frame(width: 128, height: 128)
                        .foregroundStyle(Color.white)
                    Text("入浴時間が長くなっています\n確認してください")
                        .font(.title)
                        .foregroundStyle(Color.white)
                        .bold()
                        .multilineTextAlignment(.center)
                        .padding()
                }
                .frame(maxWidth: .infinity, maxHeight: .infinity)
                
                Button {
                    makeCall()
                } label: {
                    HStack {
                        Image(systemName: "phone.fill")
                            .foregroundStyle(.red)
                        Text("緊急連絡先へ連絡する")
                            .foregroundStyle(Color.red)
                            .bold()
                    }
                    .padding(24)
                    .background(Color.white)
                    .cornerRadius(16)
                    .padding(24)
                }
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
    HomeAlertView(
        namespace: Namespace().wrappedValue,
        showingAlertView: .constant(true),
        makeCall: { print("makeCall") }
    )
}

#Preview {
    HomeView(viewModel: .init())
}
