//
//  ContentView.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import SwiftUI

struct ContentView: View {
    @ObservedObject var transcriber = Transcriber()
    
    var body: some View {
        VStack {
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
