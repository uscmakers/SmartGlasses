#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

// define the number of bytes you want to access
#define EEPROM_SIZE 1

// Pin definition for camera model
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
 
RTC_DATA_ATTR int pictureNumber = 0;

void take_photo(){
  pinMode(GPIO_NUM_13, INPUT_PULLUP);  //set trigger to end deep sleep
  
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

  pinMode(4, INPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_dis(GPIO_NUM_4);
  
  config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;
  config.fb_count = 2;
 
  // initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) return;

  //set image parameters
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 2);
  s->set_contrast(s, -0.5);
  s->set_saturation(s, 0);
 
  //check SD card 
  if(!SD_MMC.begin()) return;
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE) return;
  
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.write(0, pictureNumber);
  EEPROM.commit();
  pictureNumber++;
  Serial.println(EEPROM.read(0));
  Serial.println(pictureNumber);

  camera_fb_t * fb = NULL;
  if (EEPROM.read(0) == 0){
    return;
  }
  else{
    //take picture 
    fb = esp_camera_fb_get();
    if(!fb) return;
  }
  
   
  // create new path
  String path = "/picture" + String(pictureNumber) +".jpg";
  fs::FS &fs = SD_MMC;

  //create and save file
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file) return;
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
  }
  file.close();
  esp_camera_fb_return(fb);
    
}
  
void setup() {
  take_photo();
  pinMode(4, OUTPUT);
  rtc_gpio_hold_en(GPIO_NUM_4);
  
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);
  esp_deep_sleep_start();
} 
 
void loop() {
 
}
