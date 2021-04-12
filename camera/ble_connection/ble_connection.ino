#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

bool deviceConnected = false;
int data_val = 0;

class SmartGlassesCallbacks: public BLEServerCallbacks {
  void onConnect (BLEServer* pServer){
    deviceConnected = true;
  }

  void onDisconnect (BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("BLE Started");

  //create BLE device
  BLEDevice::init("SmartGlasses");

  //create BLE server
  pServer = BLEDevice::createServer();
  pServer -> setCallbacks(new SmartGlassesCallbacks);

  //create BLE service
  pService = pServer -> createService(SERVICE_UUID);

  //create BLE characteristic
  pCharacteristic = pService -> createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  //BLE2902 to notify
  pCharacteristic -> addDescriptor (new BLE2902());

  //start BLE
  pService -> start();
  pServer -> getAdvertising() -> start();
}

void loop() {

  //send data
  if (deviceConnected) {
    data_val = random(0,10);

    pCharacteristic -> setValue(data_val);
    pCharacteristic -> notify();
    delay(1000);
  }
  
}
