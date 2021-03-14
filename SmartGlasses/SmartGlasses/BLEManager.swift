//
//  BLEManager.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/10/21.
//

import Foundation
import CoreBluetooth

struct Peripheral: Identifiable {
    let id: Int
    let name: String
    let rssi: Int
    let uuid: String
}

class BLEManager: NSObject, ObservableObject, CBPeripheralDelegate, CBCentralManagerDelegate {
    
    var myCentral: CBCentralManager!
    var smartGlasses: CBPeripheral!
    
    @Published var isSwitchedOn = false
    @Published var peripherals = [Peripheral]()
    
    override init() {
        super.init()
        
        myCentral = CBCentralManager(delegate: self, queue: nil)
        myCentral.delegate = self
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        var peripheralName: String!
        
        if let name = advertisementData[CBAdvertisementDataLocalNameKey] as? String {
            peripheralName = name
        }
        else {
            peripheralName = "Unknown"
        }
        
        let newPeripheral = Peripheral(id: peripherals.count, name: peripheralName, rssi: RSSI.intValue, uuid: peripheral.identifier.uuidString)
        print(newPeripheral)
        peripherals.append(newPeripheral)
        
        
        // connect to smart glasses
        myCentral.stopScan()
        smartGlasses = peripheral
        smartGlasses.delegate = self
        myCentral.connect(peripheral)
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected")
        peripheral.discoverServices(nil)
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            isSwitchedOn = true
        }
        else {
            isSwitchedOn = false
        }
    }
    
    // Handles discovery event
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let services = peripheral.services {
            for service in services {
                print("BLE Service found")
                peripheral.discoverCharacteristics(nil, for: service)
            }
        }
    }
    
    // Handling discovery of characteristics
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if let characteristics = service.characteristics {
            for characteristic in characteristics {
//                print("[Characteristic \(characteristic.uuid)] \(String(describing: characteristic.value))")
                peripheral.readValue(for: characteristic)
            }
        }
    }
    
    // Read value
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        print("[Characteristic \(characteristic.uuid)] \(String(data: characteristic.value!, encoding: String.Encoding.ascii) ?? "err")")
    }
    
    func getCharacteristics() {
        print("Getting characteristics")
        smartGlasses.discoverServices(nil)
    }
    
    func startScanning() {
        let smartGlassesCBUUID = CBUUID(string: "4fafc201-1fb5-459e-8fcc-c5c9c331914b")
        print("startScanning")
        //        myCentral.scanForPeripherals(withServices: nil, options: nil)
        myCentral.scanForPeripherals(withServices: [smartGlassesCBUUID], options: nil)
    }
    
    func stopScanning() {
        print("stopScanning")
        myCentral.stopScan()
    }
}
