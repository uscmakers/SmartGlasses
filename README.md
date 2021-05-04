# SmartGlasses
Smart glasses that can perform audio recognition and computer vision

### About

This system uses an iOS application, ESP-32 CAM, and Node.js server. The iOS application connects to the ESP-32 CAM through BLE and the Node.js server through HTTPS. The application is responsible for receiving audio and image files, performing voice recognition, labeling images, and playing text-to-speech. The app accomplishes voice recognition through the built-in iOS Speech framework. The app accomplishes image labeling through Google's ML Kit.

The server handles the voice recognition response. When the app finishes voice recognition, it sends the recognized text to the server via a `POST` request. The server proxies the request to DialogFlow. DialogFlow acts as a chat bot that can interpret the text and categorize the text to different intents. DialogFlow also returns a response string which the iOS application reads. The bot is set up to handle 3 different intents: `turn_light_on`, `turn_light_off`, and `take_picture`. The `turn_light_on` intent turns the ESP-32 CAM flashlight on. The `turn_light_off` intent turns the ESP-32 CAM flashlight off. The `take_picture` intent captures an image from the ESP-32 and sends the picture to the phone.

Finally, the ESP-32 CAM uses BLE characteristics and callbacks to handle the phone's requests. When the phone needs to get an image from the ESP-32 CAM, the app writes to the image characteristic. The ESP-32 CAM receives the write and executes the image characteristic callback function. Because the callback is executed by interrupt, the ESP-CAM cannot flush BLE at the same time. That means we cannot send a ton of data in the callback (like an image). Sending a lot of data causes the ESP-32 to hang on the interrupt, and the device will not return to the main loop. Instead of sending data through the callback, the device uses write and receive flags, and the main loop handles the transmission. The light on and off consists of one BLE characteristic and callback. A phone just needs to write `on` or `off` to the device to switch the internal light on or off.

### Requirements

* XCode
* Node.js
* npm
* ngrok (optional)
* CocoaPods
* iPhone
* ESP-32 CAM
* Arduino

### How to Run

**Server**

Under `server/`, install the necessary packages.

```sh
$ npm install
```

Download your Google Cloud credentials (JSON) that are necessary to access DialogFlow. I used the `gcloud` tool since the Google Cloud portal is confusing.

```sh
$ gcloud iam service-accounts keys create service-account.json --iam-account=<YOUR IAM ACCOUNT>
```

Run the server.

```sh
$ npm start
```

*(optional but highly recommended)*

Start a ngrok local tunnel so the application can connect to the server.

```sh
ngrok http 3000
```

Copy the URL with `https://`. This will be necessary for the iOS application to connect.

*If you decide to skip this step, you will need to host the server somewhere (AWS, DigitalOcean, Google Cloud, Azure, etc) and get the URL that way*

**iOS**

In `SmartGlasses/`, install the necessary libraries for the iOS application.

```sh
$ pod install
```

Open the project workspace, `SmartGlasses/SmartGlasses.xcworkspace` in XCode. Open `Transcriber.swift`. On line 14, replace

```swift
...
class Transcriber: ObservableObject {
    let baseURL = "https://404fa16b4356.ngrok.io"
    
    @Published var transcription = ""
    ...
```

with


```swift
...
class Transcriber: ObservableObject {
    let baseURL = "<YOUR URL HERE>"
    
    @Published var transcription = ""
    ...
```

You can now connect your iOS device and press the `Run` button at the top left to run the application on your device.

**ESP-32 CAM**

Follow these [instructions](https://randomnerdtutorials.com/program-upload-code-esp32-cam/) to set up the Arduino environment so you can run the ESP-32 CAM sketch. It will also show you how to upload the sketch to the ESP-32 CAM.

Upload the sketch from `Device/` to the ESP-32 CAM.

**Arduino**

Install this [library](https://github.com/TMRh20/TMRpcm) for audio recording to SD cards from an Arduino. Upload the sketch under `record_audio.ino` to the Arduino. Connect the microphone output to `A0`. Connect the Arduino RX/TX to the ESP-32 CAM TX/RX, respectively.

### How to Use

Ensure the Arduino and ESP-32 CAM are on. Open the iOS application. Click connect. The device should pair with the app. You can now click the button on the device to activate the smart asssistant.
