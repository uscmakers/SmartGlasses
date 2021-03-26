//
//  ContentView.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/5/21.
//

import SwiftUI

struct ContentView: View {
    let recognizer = Recognizer()
    
    var body: some View {
        VStack {
            Text("Hello, world!")
                .padding()
            Button(action: {
                recognizer.detectLabels()
            }) {
                Text("Detect labels")
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
