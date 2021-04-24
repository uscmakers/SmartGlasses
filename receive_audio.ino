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
  //digitalWrite(ledPin, HIGH);
  Serial.begin(115200);

}

void loop() {
  if(Serial.available()) {
    
    //digitalWrite(ledPin, HIGH);
    if(!flag) {
      //digitalWrite(ledPin, HIGH);
      //Serial.readString();
      //if(Serial.readString()=="START\r\n") {
      digitalWrite(ledPin, HIGH);
      //digitalWrite(ledPin, HIGH);
      flag = true;
      myFile = SD.open("received.wav", FILE_WRITE);
      //}
    }
    if(flag) {
      myFile.write(Serial.read());
      
      
    } 
    start = millis();
    

  } else {
    if(flag && millis()-start >= 500) {
      //digitalWrite(ledPin, LOW);
      myFile.close();
      flag = false;
      digitalWrite(ledPin, LOW);
    }
  }
  
  
}
