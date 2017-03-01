# IoT Hub Feather HUZZAH Client application 
[![Build Status](https://travis-ci.com/Azure-Samples/iot-hub-feather-huzzah-client-app.svg?token=5ZpmkzKtuWLEXMPjmJ6P&branch=master)](https://travis-ci.com/Azure-Samples/iot-hub-feather-huzzah-client-app)

> This repo contains the source code to help you get familiar with Azure IoT using the Azure IoT Adafruit Feather HUZZAH ESP8266 Starter Kit. You will find the [lesson-based tutorials on Azure.com](#).

This repo contains an arduino application that runs on board feather HUZZAH ESP8266 with a DHT22 temperature&humidity sensor, and then sends these data to your IoT hub. At the same time, this application receives Cloud-to-Device message from your IoT hub, and takes actions according to the C2D command. 

## Install board with your Arduino IDE
1. Start Arduino and open Preferences window.
2. Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
3. Open Boards Manager from `Tools > Board` menu and install esp8266 platform 2.2.0 or later
4. Select your ESP8266 board from `Tools > Board` menu after installation

## Install libraries
Install the following libraries from `Sketch -> Include library -> Manage libraries`

* AzureIoTHub
* AzureIoTUtility
* AzureIoTProtocol_HTTP
* ArduinoJson
* DHT sensor library

## Connect your sensor with your board
### Connect with a physical DHT22 sensor
You can follow the image to connect your DHT22 with your feather HUZZAH ESP8266.

![DHT22](#)

### DON'T HAVE A PHYSICAL DHT22?
You can use the application to simulate temperature&humidity data and send to your IoT hub.
1. Open the `app/config.h` file.
2. Change the `SIMULATED_DATA` value from `false` to `true`.

## Configure and run sample application
Upload the `app.ino` to your board.

### Input your credential information
After you successfully upload the code to your board. You will see some prompt, input your credential information according to the prompts.

### Send Cloud-to-Device command
You can send a json message to your device as one of the followings:
 * `{"command": "blink"}` - this will blink the LED on your board once.
 * `{"command": "start"}` - this will start to read temperature and humidity data from your DHT22 sensor or simulated data and send to your IoT hub.
 * `{"command": "stop"}`  - this will stop sending message to your IoT hub.
