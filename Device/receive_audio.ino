/*
Steps:
1. Edit pcmConfig.h
    a: On Uno or non-mega boards, #define buffSize 128. May need to increase.
    b: Uncomment #define ENABLE_RECORDING and #define BLOCK_COUNT 10000UL

2. Usage is as below. See https://github.com/TMRh20/TMRpcm/wiki/Advanced-Features#wiki-recording-audio for
   additional informaiton.
*/

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
  //audio.CSPin = SD_ChipSelectPin;
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

}

void loop() {
  if(Serial.available()) {
    if(!flag) {
      //Serial.readString();
      if(Serial.readString()=="START") {
        
        flag = true;
        myFile = SD.open("received.wav", FILE_WRITE);
      }
    }
    if(flag) {
      myFile.write(Serial.read());
      digitalWrite(ledPin, HIGH);
    } 
    start = millis();
    

  } else {
    if(flag && millis()-start >= 500) {
      myFile.close();
      flag = false;
    }
  }
  
  
}
