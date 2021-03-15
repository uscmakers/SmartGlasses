/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LED_BUILTIN 4

class BLECallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        Serial.println(value.c_str());
        
        if (value == "on") {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("turning on led");
        } else if (value == "off") {
          Serial.println("turning off led");
          digitalWrite(LED_BUILTIN, LOW);
        }
        
        Serial.println("*********");
      }
    }
};

BLECharacteristic pCharacteristic(CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  pinMode (LED_BUILTIN, OUTPUT);

  BLEDevice::init("Smart Glasses");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->addCharacteristic(&pCharacteristic);
  pCharacteristic.setCallbacks(new BLECallbacks());

  pCharacteristic.setValue("Commands");
  pService->start();
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(2000);
//  pCharacteristic.setValue("Commands");
//  pCharacteristic.notify();
}
