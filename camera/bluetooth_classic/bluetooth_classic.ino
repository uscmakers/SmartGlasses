#include "BluetoothSerial.h"
  
BluetoothSerial SerialBT;
   
void setup()
{
  SerialBT.begin("ESP32test");
  delay(1000);
}
   
void loop()
{
  String inputFromOtherSide;
  if (SerialBT.available()) {
    inputFromOtherSide = SerialBT.readString();
    SerialBT.println("You had entered: ");
    SerialBT.println(inputFromOtherSide);
  }
}
