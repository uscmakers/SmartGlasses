
#include <SD.h>
#include <SPI.h>
#include <TMRpcm.h>
#define SD_ChipSelectPin 10
TMRpcm audio;
volatile int audiofile = 0;
volatile unsigned long i = 0;
volatile bool recmode = 0;
volatile bool toSend = false;

void setup() {
  // Microphone pin
  pinMode(A0, INPUT);
  
  // LED pin
  pinMode(4, OUTPUT);

  // Button pullup
  pinMode(2, INPUT_PULLUP);

  // attach button to interrupt
  attachInterrupt(digitalPinToInterrupt(2), button, LOW);
  SD.begin(SD_ChipSelectPin);
  audio.CSPin = SD_ChipSelectPin;

  Serial.begin(115200);

}

void loop() {
  
  if(toSend) {

    File dataFile = SD.open("AUDIO.WAV");
    // if the file is available, write to it:
    if (dataFile) {
      
      digitalWrite(4, HIGH);
      delay(100);
      while (dataFile.available()) {
        Serial.write(dataFile.read());
      }
      dataFile.close();
      digitalWrite(4, LOW);
    }

    toSend = false;
  }

  
}

void button() {
  while (i < 300000) {
    i++;
  }

  i = 0;
  if (recmode == 0) {
    recmode = 1;

    audio.startRecording("audio.wav", 32000, A0);
      
  }
  else {
    recmode = 0;
    audio.stopRecording("audio.wav");
    toSend = true;
    
      
  }
}
