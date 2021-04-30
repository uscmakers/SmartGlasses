#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "esp_camera.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define LED_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define IMAGE_CHARACTERISTIC_UUID "1b9229e8-de4f-43f2-90d7-58a11c00517e"
#define ACK_CHARACTERISTIC_UUID "dec0bc71-3b11-4815-9994-12bda9a9fd2b"

#define LED_BUILTIN 4

#define BUTTON GPIO_NUM_13

camera_fb_t* capture_img();

camera_fb_t* fb;
int send_image = 0;

// send image data
BLECharacteristic imageCharacteristic(IMAGE_CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

// acknowledge image has been fully sent
BLECharacteristic ackCharacteristic(ACK_CHARACTERISTIC_UUID, 
  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

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
  
  pinMode(BUTTON, INPUT_PULLUP);

    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  //set camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
//  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 10;
//  } else {
//    config.frame_size = FRAMESIZE_SVGA;
//    config.jpeg_quality = 12;
//    config.fb_count = 1;
//  }

//  rtc_gpio_hold_dis(GPIO_NUM_4);

  // initialize camera
  esp_err_t err = esp_camera_init(&config);
  //if (err != ESP_OK) return NULL;
}

int i = 0;

void loop() {
  if (digitalRead(BUTTON) == 0){
    fb = capture_img();
    Serial.println("Took pic");
    send_image = 1;
  }
  
  if(send_image) {
    const int bytes = 160;
    const int iterations = fb->len / bytes;
    Serial.println("transferring pic");
    Serial.println(iterations);
    if (i < iterations) {
      imageCharacteristic.setValue(fb->buf, bytes);
//      imageCharacteristic.notify();
      fb->buf += bytes;
      Serial.print("[");
      Serial.print(i);
      Serial.print("/");
      Serial.print(iterations);
      Serial.print("]");
      ++i;
    } else {
      if(fb->len % bytes) {
//        imageCharacteristic.setValue(fb->buf, fb->len % bytes);
//        imageCharacteristic.notify();
      }
      // data not required
//      ackCharacteristic.setValue("done");
//      ackCharacteristic.notify();
        
      Serial.println("Finished");
      
      // return frame buffer
      esp_camera_fb_return(fb);
      Serial.println("Frame restored!");
      send_image = 0;
      i = 0;
    }
  }
  delay(10);
}
