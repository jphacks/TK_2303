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
                makeCall: viewModel.makeCall,
                makeImpact: viewModel.makeImpact
            )
        } else {
            ZStack {
                Color.backgroundColor
                    .ignoresSafeArea()
                ZStack(alignment: .bottom) {
                    VStack(spacing: 0) {
                        headerView()
                        ScrollView {
                            bathStatusView()
                            HomeNotificationListView(notifications: viewModel.notifications)
                        }
                        .refreshable {
                            Task { @MainActor in
                                await viewModel.refresh()
                            }
                        }
                    }
                    HeatShockPopupView(showingPopup: viewModel.isHighHeatShockPossiblity)
                }
            }
            .onAppear {
                viewModel.onAppear()
            }
            .onDisappear {
                viewModel.onDisapear()
            }
            .task {
                await viewModel.refresh()
            }
        }
    }
    
    @ViewBuilder
    func headerView() -> some View {
        VStack(spacing: 0) {
            HStack {
                Text("ホッとお風呂")
                    .font(.title)
                    .bold()
                    .foregroundStyle(Color.font)
                Spacer()
                NavigationLink {
                    SettingView()
                } label: {
                    Image(systemName: "gearshape.fill")
                        .resizable()
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
        }
    }
    
    @ViewBuilder
    func bathStatusView() -> some View {
        ZStack(alignment: .topTrailing) {
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
                        Text(String(format: "%.1f", viewModel.temperature ?? 0.0) + "°")
                            .font(.title2)
                            .foregroundStyle(Color.black.opacity(0.7))
                    }
                    VStack {
                        Text("湿度")
                            .font(.body)
                            .foregroundStyle(Color.black.opacity(0.6))
                        Text(String(format: "%.1f", viewModel.humidity ?? 0.0) + "°")
                            .font(.title2)
                            .foregroundStyle(Color.black.opacity(0.7))
                    }
                }
            }
            .frame(maxWidth: .infinity)
            .padding(24)
            .background(Color.white)
            NavigationLink {
                ScrollView {
                    VStack {
                        HomeNotificationListView(notifications: viewModel.notifications)
                        Spacer()
                    }
                }
            } label: {
                Image(systemName: "bell.fill")
                    .resizable()
                    .matchedGeometryEffect(id: "bell", in: namespace)
                    .frame(width: 24, height: 24)
                    .foregroundStyle(.font)
                    .padding(24)
            }
        }
    }
}

#Preview {
    struct PreviewWrapper: View {
        class AppDataStoreMock: AppDataStore {
            override func refresh() async {}
        }
        var appDataStore = AppDataStoreMock()
        init(appDataStore: AppDataStoreMock = AppDataStoreMock()) {
            self.appDataStore = appDataStore
            appDataStore.storedData = .init(
                bathStatusJson: .init(status: .outBath),
                sensorDatasJson: .init(
                    temperature: 24.0,
                    humidity: 70.8,
                    pressure: 10,
                    lastUpdate: "hello"
                ),
                historyJson: .init(
                    history: [
                        .init(
                            action: .goIntoBath,
                            date: "2023-11-15T02:30:40+09:00"
                        ),
                        .init(
                            action: .goOutOfBath,
                            date: "2023-11-15T02:50:40+09:00"
                        ),
                        .init(
                            action: .goIntoBath,
                            date: "2023-11-16T02:30:40+09:00"
                        ),
                        .init(
                            action: .goOutOfBath,
                            date: "2023-11-16T02:50:40+09:00"
                        ),
                    ]
                )
            )
            
        }
//        struct StoredData {
//            var bathStatusJson: BathStatusJson?
//            var sensorDatasJson: SensorDataJson?
//            var historyJson: HistoryJson?
//            var phoneNumberJson: PhoneNumberJson?
//        }
        var body: some View {
            HomeView(viewModel: .init(appDataStore: appDataStore))
        }
    }
    return PreviewWrapper()
}
