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
#include <TMRpcm.h>
#define SD_ChipSelectPin 10
TMRpcm audio;
volatile int audiofile = 0;
volatile unsigned long i = 0;
volatile bool recmode = 0;

void setup() {
  // Microphone pin
  pinMode(A0, INPUT);

  // LED pin
  pinMode(13, OUTPUT);

  // Button pullup
  pinMode(2, INPUT_PULLUP);

  // attach button to interrupt
  attachInterrupt(digitalPinToInterrupt(2), button, LOW);
  SD.begin(SD_ChipSelectPin);
  audio.CSPin = SD_ChipSelectPin;

  Serial.begin(115200);

}

void loop() {
}

void button() {
  Serial.println("starting 1");
  while (i < 300000) {
    i++;
  }

  Serial.println("starting 2");
  i = 0;
  if (recmode == 0) {
    recmode = 1;
    audiofile++;
    digitalWrite(13, HIGH);
    
    audio.startRecording("audio.wav", 32000, A0);
      
  }
  else {
    recmode = 0;
    digitalWrite(13, LOW);
    audio.stopRecording("audio.wav");

    Serial.println("recording ssaved");

    File dataFile = SD.open("AUDIO.WAV");
    // if the file is available, write to it:
    if (dataFile) {
      while (dataFile.available()) {
        Serial.println("START");
        Serial.write(dataFile.read());
      }
      Serial.println("END");
      dataFile.close();
    }
      
  }
}
