//
//  HomeAlertView.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/10/29.
//

import SwiftUI

struct HomeAlertView: View {
    var namespace: Namespace.ID
    @Binding var showingAlertView: Bool
    var makeCall: () -> ()
    var makeImpact: () -> ()
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
                    Text("大きな音が検知されました\n確認してください")
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
                    .padding(.bottom, 24)
                }
            }
        }.onAppear{
            DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) {
                makeImpact()
            }
        }
    }
}

#Preview {
    HomeAlertView(
        namespace: Namespace().wrappedValue,
        showingAlertView: .constant(true),
        makeCall: { print("makeCall") },
        makeImpact: { print("makeImpact") }
    )
}
