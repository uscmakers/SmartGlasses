#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_camera.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define LED_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define IMAGE_CHARACTERISTIC_UUID "1b9229e8-de4f-43f2-90d7-58a11c00517e"
#define ACK_CHARACTERISTIC_UUID "dec0bc71-3b11-4815-9994-12bda9a9fd2b"

#define LED_BUILTIN 4

camera_fb_t* capture_img();

// control leds
BLECharacteristic ledCharacteristic(LED_CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

// send image data
BLECharacteristic imageCharacteristic(IMAGE_CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

// acknowledge image has been fully sent
BLECharacteristic ackCharacteristic(ACK_CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

// control LED
class LEDCallback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        Serial.println(value.c_str());
        
        if (value == "on") {
          digitalWrite(LED_BUILTIN, HIGH);
          ledCharacteristic.setValue("The LED is on");
          ledCharacteristic.notify();
          Serial.println("turning on led");
        } else if (value == "off") {
          Serial.println("turning off led");
          ledCharacteristic.setValue("The LED is off");
          ledCharacteristic.notify();
          digitalWrite(LED_BUILTIN, LOW);
        }
        
        Serial.println("*********");
      }
    }
};

camera_fb_t* fb;
int send_image = 0;

// send an image
class ImageCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) {
    Serial.println("getting image...");
    fb = capture_img();
    Serial.println("took pic");
    send_image = 1;
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  
  pinMode(LED_BUILTIN, OUTPUT);

  BLEDevice::init("Smart Glasses");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pService->addCharacteristic(&ledCharacteristic);
  ledCharacteristic.setCallbacks(new LEDCallback());
  ledCharacteristic.setValue("The LED is off");

  pService->addCharacteristic(&imageCharacteristic);
  imageCharacteristic.setCallbacks(new ImageCallback());

  pService->addCharacteristic(&ackCharacteristic);
  
  pService->start();
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

int i = 0;

void loop() {
  if(send_image) {
    const int bytes = 160;
    const int iterations = fb->len / bytes;
    Serial.println("transferring pic");
    if (i < iterations) {
      imageCharacteristic.setValue(fb->buf, bytes);
      imageCharacteristic.notify();
      fb->buf += bytes;
      Serial.print("[");
      Serial.print(i);
      Serial.print("/");
      Serial.print(iterations);
      Serial.print("]");
      ++i;
    } else {
      if(fb->len % bytes) {
        imageCharacteristic.setValue(fb->buf, fb->len % bytes);
        imageCharacteristic.notify();
      }
      // data not required
      ackCharacteristic.setValue("done");
      ackCharacteristic.notify();
        
      Serial.println("Finished");
      
      // return frame buffer
      esp_camera_fb_return(fb);
      Serial.println("Frame restored!");
      send_image = 0;
    }
  }
  delay(10);
}
