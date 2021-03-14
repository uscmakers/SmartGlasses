//
//  ContentView.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import SwiftUI

struct ContentView: View {
    @ObservedObject var bleManager = BLEManager()
    @ObservedObject var transcriber = Transcriber()
    
    var body: some View {
        VStack {
            Text("Bluetooth Devices")
                .font(.largeTitle)
                .frame(maxWidth: .infinity, alignment: .center)
            
            List(bleManager.peripherals) { peripheral in
                HStack {
                    Text(peripheral.name)
                    Spacer()
                    Text(String(peripheral.rssi))
                }
            }.frame(height: 100)

            Text("STATUS")
                .font(.headline)

            // Status goes here
            if bleManager.isSwitchedOn {
                Text("Bluetooth is switched on")
                    .foregroundColor(.green)
            }
            else {
                Text("Bluetooth is NOT switched on")
                    .foregroundColor(.red)
            }

            HStack {
                VStack (spacing: 10) {
                    Button(action: {
                        self.bleManager.startScanning()
                    }) {
                        Text("Start Scanning")
                    }
                    Button(action: {
                        self.bleManager.stopScanning()
                    }) {
                        Text("Stop Scanning")
                    }
                }.padding()
                
                VStack (spacing: 10) {
                    Button(action: {
                        self.bleManager.getCharacteristics()
                    }) {
                        Text("Get Characteristics")
                    }
                }.padding()
            }
            
            // voice
            
            if !transcriber.transcription.isEmpty {
                Text("You: \(transcriber.transcription)")
                    .padding()
            } else {
                Text("Transcription appears here")
                    .padding()
            }
            if !transcriber.botResponse.isEmpty {
                Text("Bot: \(transcriber.botResponse)")
                    .foregroundColor(/*@START_MENU_TOKEN@*/.blue/*@END_MENU_TOKEN@*/)
                    .padding()
            } else {
                Text("Bot response appears here")
                    .padding()
            }
            
            Button(action: transcriber.toggleRecording) {
                if transcriber.isRecording {
                    Text("Stop Recording")
                        .foregroundColor(.blue)
                        .padding()
                } else {
                    Text("Start Recording")
                        .foregroundColor(.red)
                        .padding()
                }
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
