//
//  AppDataStore.swift
//  BathCare
//
//  Created by 五十嵐諒 on 2023/11/09.
//

import Foundation
class AppDataStore {
    static let shared = AppDataStore()
    @Published var storedData: StoredData = .init()
    struct StoredData {
        var bathStatusJson: BathStatusJson?
        var sensorDatasJson: SensorDataJson?
        var historyJson: HistoryJson?
        var phoneNumberJson: PhoneNumberJson?
    }
    func refresh() async {
        await fetchBathStatus()
        await fetchSensorDataJson()
        await fetchPhoneNumberJson()
        await fetchHistoryJson()
    }

    func fetchBathStatus() async {
        do {
            let service = BathStatusService()
            let response = try await service.fetch()
            DispatchQueue.main.async {
                self.storedData.bathStatusJson = response
            }
        } catch(let error) {
            print(error)
        }
    }

    func fetchSensorDataJson() async {
        do {
            let service = SensorDataService()
            let response = try await service.fetch()
            DispatchQueue.main.async {
                self.storedData.sensorDatasJson = response
            }
        } catch(let error) {
            print(error)
        }
    }
    
    func fetchHistoryJson() async {
        do {
            let service = HistoryService()
            let response = try await service.fetch()
            DispatchQueue.main.async {
                self.storedData.historyJson = response
            }
        } catch(let error) {
            print(error)
        }
    }

    func fetchPhoneNumberJson() async {
        do {
            let service = PhoneNumberService()
            let response = try await service.fetch()
            DispatchQueue.main.async {
                self.storedData.phoneNumberJson = response
            }
        } catch(let error) {
            print(error)
        }
    }
}
