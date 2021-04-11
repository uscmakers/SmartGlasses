//
//  BLEManager.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/10/21.
//

import UIKit
import Foundation
import CoreBluetooth

struct Peripheral: Identifiable {
    let id: Int
    let name: String
    let rssi: Int
    let uuid: String
}

class BLEManager: NSObject, ObservableObject, CBPeripheralDelegate, CBCentralManagerDelegate {
    let SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
    let LED_CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
    let IMAGE_CHARACTERISTIC_UUID = "1b9229e8-de4f-43f2-90d7-58a11c00517e"
    let ACK_CHARACTERISTIC_UUID = "dec0bc71-3b11-4815-9994-12bda9a9fd2b"
    
    var myCentral: CBCentralManager!
    var smartGlasses: CBPeripheral!
    var characteristics: [String: CBCharacteristic] = [:]
    var characteristicsValues: [String: Data] = [:]
    var imageData: Data = Data()
    
    @Published var image: UIImage!
    @Published var isSwitchedOn = false
    @Published var peripherals = [Peripheral]()
    @Published var characteristicValue = ""
    
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
                self.characteristics[characteristic.uuid.uuidString] = characteristic
                peripheral.readValue(for: characteristic)
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateNotificationStateFor characteristic: CBCharacteristic, error: Error?) {
        switch characteristic.uuid.uuidString.lowercased() {
        case IMAGE_CHARACTERISTIC_UUID:
            print("subbed to image")
            break
        case ACK_CHARACTERISTIC_UUID:
            // convert image data to image
            print("subbed to ack")
            break
        default:
            print("subscribed to default")
        }
    }
    
    // Read value
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
//        print("[Characteristic \(characteristic.uuid)] \(String(describing: characteristic.value))")
//        characteristicValue = String(data: characteristic.value!, encoding: String.Encoding.ascii) ?? "err"
        switch characteristic.uuid.uuidString.lowercased() {
            case IMAGE_CHARACTERISTIC_UUID:
//                print("getting image")
                imageData.append(characteristic.value!)
                break
            case ACK_CHARACTERISTIC_UUID:
                // convert image data to image
//                print("finished getting image")
                self.image = UIImage.init(data: imageData)
                imageData.removeAll()
                break
            default:
                self.characteristicsValues[characteristic.uuid.uuidString] = characteristic.value
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didWriteValueFor characteristic: CBCharacteristic, error: Error?) {
    }
    
    func write(text: String, uuid: String) {
        let data = text.data(using: .utf8)!
        smartGlasses.writeValue(data, for: self.characteristics[uuid.uppercased()]!, type: .withResponse)
    }
    
    func startScanning() {
        let smartGlassesCBUUID = CBUUID(string: SERVICE_UUID)
        print("startScanning")
        //        myCentral.scanForPeripherals(withServices: nil, options: nil)
        myCentral.scanForPeripherals(withServices: [smartGlassesCBUUID], options: nil)
    }
    
    func stopScanning() {
        print("stopScanning")
        myCentral.stopScan()
    }
}
