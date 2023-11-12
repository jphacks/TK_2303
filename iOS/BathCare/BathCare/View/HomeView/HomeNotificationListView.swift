//
//  HomeNotificationListView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import SwiftUI

struct HomeNotificationListView: View {
    var notifications: [ActionNotification]
    var body: some View {
        VStack {
            Divider()
            ForEach(notifications.sorted(by: { $0.date > $1.date }), id: \.date) { notification in
                switch notification.action {
                case .goIntoBath:
                    HStack(spacing: 16) {
                        ZStack {
                            Circle()
                                .frame(width: 64, height: 64)
                                .foregroundStyle(Color.gray.opacity(0.2))
                            Image(systemName: "bathtub.fill")
                                .resizable()
                                .frame(width: 48, height: 32)
                                .foregroundStyle(.font)
                                .padding(.top, 8)
                        }
                        VStack(alignment: .leading) {
                            Text("お風呂に入ったようです")
                            Text(notification.dateAgo ?? "")
                                .font(.caption)
                                .foregroundStyle(Color.black.opacity(0.5))
                        }
                        Spacer()
                    }
                    .padding(8)
                    Divider()
                case .goOutOfBath:
                    HStack(spacing: 16) {
                        ZStack {
                            Circle()
                                .frame(width: 64, height: 64)
                                .foregroundStyle(Color.gray.opacity(0.2))
                            Image(systemName: "bathtub.fill")
                                .resizable()
                                .frame(width: 48, height: 32)
                                .foregroundStyle(.gray)
                                .padding(.top, 8)
                        }
                        VStack(alignment: .leading) {
                            Text("お風呂を出たようです")
                            Text(notification.dateAgo ?? "")
                                .font(.caption)
                                .foregroundStyle(Color.black.opacity(0.5))
                        }
                        Spacer()
                    }
                    .padding(8)
                    Divider()
                case .becomeDanger:
                    HStack(spacing: 16) {
                        ZStack {
                            Circle()
                                .frame(width: 64, height: 64)
                                .foregroundStyle(Color.gray.opacity(0.2))
                            Image(systemName: "bathtub.fill")
                                .resizable()
                                .frame(width: 48, height: 32)
                                .foregroundStyle(.red)
                                .padding(.top, 8)
                        }
                        VStack(alignment: .leading) {
                            Text("危険が検知されました！\n注意してください")
                            Text(notification.dateAgo ?? "")
                                .font(.caption)
                                .foregroundStyle(Color.black.opacity(0.5))
                        }
                        Spacer()
                    }
                    .padding(8)
                    Divider()
                }
            }
        }
        .padding(.horizontal, 16)
        .background(Color.white)
    }
}

#Preview {
    HomeNotificationListView(notifications: [])
}
