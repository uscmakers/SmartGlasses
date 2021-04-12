//
//  Recognizer.swift
//  SmartGlasses
//
//  Created by Devin Mui on 3/25/21.
//

import Foundation
import MLKit

class Recognizer : NSObject, ObservableObject {
//    func detectLabels(image: UIImage) {
    func detectLabels() {
        let filePath = Bundle.main.path(forResource: "boat", ofType: "jpeg")!
        let image: UIImage! = UIImage.init(contentsOfFile: filePath)
        
        let visionImage = VisionImage(image: image)
        visionImage.orientation = image.imageOrientation
        let options: CommonImageLabelerOptions! = ImageLabelerOptions()
        let labeler = ImageLabeler.imageLabeler(options: options)
        labeler.process(visionImage) {
            labels, error in guard error == nil, let labels = labels else { return }
            for label in labels {
                let labelText = label.text
                let confidence = label.confidence
                let index = label.index
                print("[Label] \(index) \(labelText) \(confidence)")
            }
        }
    }
}
