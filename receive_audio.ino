#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#define SD_ChipSelectPin 10

int start = millis();
File myFile;
bool flag = false;
int ledPin = 4;

void setup() {

  SD.begin(SD_ChipSelectPin);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  if(Serial.available()) {
    

    if(!flag) {

      digitalWrite(ledPin, HIGH);

      flag = true;
      myFile = SD.open("received.wav", FILE_WRITE);

    }
    if(flag) {
      myFile.write(Serial.read());
      
      
    } 
    start = millis();
    

  } else {
    if(flag && millis()-start >= 500) {

      myFile.close();
      flag = false;
      digitalWrite(ledPin, LOW);
    }
  }
  
  
}
