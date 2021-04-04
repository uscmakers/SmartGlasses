#include <SD_MMC.h>

void setup(){

  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("Starting up");
  
  if (!SD_MMC.begin()){
    Serial.println("SD card error");
    return;
  }

  File fileToSend = SD_MMC.open("/test.txt", FILE_READ);
  
  if (!fileToSend) {
    Serial.println("No file to send");
    return;
  }
  
  while (fileToSend.available()) Serial.write(fileToSend.read());
}

void loop() { }
