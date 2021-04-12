//
//  ContentView.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import SwiftUI

struct ContentView: View {
    let recognizer = Recognizer()
    @ObservedObject var bleManager = BLEManager()
    @ObservedObject var transcriber = Transcriber()
    
    var body: some View {
        VStack {
            if bleManager.image != nil {
                Image(uiImage: bleManager.image)
            } else {
                Text("No image yet")
            }
            HStack {
                VStack (spacing: 10) {
                    Button(action: {
                        self.bleManager.startScanning()
                    }) {
                        Text("Connect")
                    }
                    Button(action: {
                        self.bleManager.stopScanning()
                    }) {
                        Text("Stop Scanning")
                    }
                }.padding()
                
                VStack (spacing: 10) {
                    Button(action: {
                        self.bleManager.write(text: "f", uuid: bleManager.IMAGE_CHARACTERISTIC_UUID)
                    }) {
                        Text("Get image")
                    }

                    Button(action: {
                        recognizer.detectLabels()
                    }) {
                        Text("Detect labels")
                            .padding()
                    }
                    
                    Button(action: {
                        self.bleManager.write(text: "on", uuid: bleManager.LED_CHARACTERISTIC_UUID)
                    }) {
                        Text("Turn on flashlight")
                    }
                    
                    Button(action: {
                        self.bleManager.write(text: "off", uuid: bleManager.LED_CHARACTERISTIC_UUID)
                    }) {
                        Text("Turn off flashlight")
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
