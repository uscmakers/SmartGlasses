//
//  ContentView.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import SwiftUI
import Speech

func onPress() {
    // ew user permissions
    SFSpeechRecognizer.requestAuthorization {
        (status) in switch status {
            case .notDetermined: print("err")
            case .restricted: print("err")
            case .denied: print("err")
            case .authorized: print("yay")
            @unknown default: print("err")
        }
    }

//    // do speech recognition
    if let speechRecognizer = SFSpeechRecognizer() {
        if speechRecognizer.isAvailable {
            let filePath = Bundle.main.path(forResource: "test", ofType: "m4a")!
            let request = SFSpeechURLRecognitionRequest(url: URL.init(string: filePath)!)
            speechRecognizer.supportsOnDeviceRecognition = true
            speechRecognizer.recognitionTask(with: request, resultHandler: {
                (result, error) in
                if let error = error {
                    // handle
                    print(error)
                    return
                }
                let result = result
                print(result?.bestTranscription.formattedString)
            })
        }
    }
}

// adapted from apple docs
func startRecording() {
    do {
    let speechRecognizer = SFSpeechRecognizer()!
    var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
    var recognitionTask: SFSpeechRecognitionTask?
    let audioEngine = AVAudioEngine()
    
    recognitionTask?.cancel()
    recognitionTask = nil
    
    // Audio session, to get information from the microphone.
    let audioSession = AVAudioSession.sharedInstance()
    try audioSession.setCategory(.record, mode: .measurement, options: .duckOthers)
    try audioSession.setActive(true, options: .notifyOthersOnDeactivation)
    let inputNode = audioEngine.inputNode
    
    recognitionRequest = SFSpeechAudioBufferRecognitionRequest()
    recognitionRequest!.shouldReportPartialResults = true
    
    // Force speech recognition to be on-device
    if #available(iOS 13, *) {
        recognitionRequest!.requiresOnDeviceRecognition = true
    }
    
    recognitionTask = speechRecognizer.recognitionTask(with: recognitionRequest!) { result, error in
        var isFinal = false
        
        if let result = result {
            isFinal = result.isFinal
            print("Text \(result.bestTranscription.formattedString)")
        }
        
        if error != nil || isFinal {
            audioEngine.stop()
            inputNode.removeTap(onBus: 0)
            
            recognitionRequest = nil
            recognitionTask = nil
        }
    }
    
    // Configure the microphone.
    let recordingFormat = inputNode.outputFormat(forBus: 0)
    inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { (buffer: AVAudioPCMBuffer, when: AVAudioTime) in
        recognitionRequest?.append(buffer)
    }
    
    audioEngine.prepare()
    try audioEngine.start()
    } catch {
        print("could not create audio player")
    }
}

struct ContentView: View {
    var body: some View {
        VStack {
            Text("Hello, world!")
                .padding()
            Button(action: onPress) {
                Text("Button")
                    .padding()
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
