//
//  transcribe.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import Foundation
import Speech
import Alamofire
import SwiftyJSON

class Transcriber: ObservableObject {
    let baseURL = "https://404fa16b4356.ngrok.io"
    
    @Published var transcription = ""
    @Published var isRecording = false
    @Published var botResponse = ""
    var recognitionTask: SFSpeechRecognitionTask? = nil
    
    func transcribeFromFile() {
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
        
        // do speech recognition
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
                    if let result = result {
                        print(result.bestTranscription.formattedString)
                        self.transcription = result.bestTranscription.formattedString
                    }
                })
            }
        }
    }
    
    func getBotResponse() {
        let parameters = [
            "text": transcription
        ]
        
        AF.request(baseURL, method: .post, parameters: parameters, encoding: JSONEncoding.default).response {
            response in
            
            if let json = response.data {
                do {
                    let data = try JSON(data: json)
                    self.botResponse = data[0]["queryResult"]["fulfillmentText"].rawValue as! String
                    print(self.botResponse)
                } catch {
                    self.botResponse = "I don't understand"
                    print("JSON error")
                }
            }
        }
    }

    // adapted from apple docs
    func toggleRecording() {
        do {
            let speechRecognizer = SFSpeechRecognizer()!
            var recognitionRequest: SFSpeechAudioBufferRecognitionRequest?
            let audioEngine = AVAudioEngine()
            
            // stop recognition
            if recognitionTask != nil {
                isRecording = false
                recognitionTask?.cancel()
                recognitionTask = nil
                // run the bot here
                getBotResponse()
                return
            }
            
            isRecording = true
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
                    self.transcription = result.bestTranscription.formattedString
                }
                
                // stop
                if error != nil || isFinal {
                    audioEngine.stop()
                    inputNode.removeTap(onBus: 0)
                    
                    recognitionRequest = nil
                    self.recognitionTask?.cancel()
                    self.recognitionTask = nil
                    self.isRecording = false
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
}
