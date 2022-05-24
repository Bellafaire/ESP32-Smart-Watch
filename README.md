# ESP32 Smartwatch
This project is a fully open source smartwatch based around the ESP32, the project is ongoing for updates see the hackaday.io page for this project (https://hackaday.io/project/168227-custom-smartwatch).
The goal of this project is to create a smartwatch that is easy to reprogram and use as an IOT controller or just as a DIY fashion statement.  
Serial communication and charging are handled on-board through a single micro-usb connector without any external dock. 
The watch is built around the ESP32 WROOM module and is programable using the espressif or Arduino IDE. 
Using the ESP32 allows for the user to develop their software while leveraging the open-source libraries and examples that are available online for quick development.

## Companion App 
The companion android app that allows the smartwatch to control media and obtain phone notifications has been split to a seperate repo and can be found [here](https://github.com/Bellafaire/Android-Companion-App-For-BLE-Devices) along with an example sketch for bluetooth interfacing using the ESP32. 
The app can be downloaded from the releases tab of the provided repo, the V1 watch firmware works only with app releases up to 1.2.0. 
For firmware versions 2 and 3 releases after 2.0.0 must be used, releases after this version use a different communication structure that has proved to be more reliable. 

## Firmware Versions
There are 3 versions of the firmware available for this project. Only V3 is in active development. 
The V1 firmware utilizes deep sleep along with the V1 android companion app, this firmware works well enough and offers the greatest power savings over any of the other options. 
V2 is a bit more animated, boasting some additional applications along with the switch to light sleep which enables faster wakeup from tap or by performing a wrist gesture. 

V3 is the current firmware, it is much more stable and expandable. 
The current V3 firmware supports: 
- App icons on home screen, tap to view the tagline of the notification
- Spotify control through tapping the spotify app icon
- built in calculator 
- Improved UI and tools to aid feature expansion. 

<img src="https://github.com/Bellafaire/ESP32-Smart-Watch/blob/master/Pictures%20and%20Videos/2022-05-22%2019.56.47.jpg?raw=true" height="600px"/>

The case is 3D printed and designed in FreeCAD, the design files are available in this repo. 

## Hardware Versions
The current hardware version for the watch is V5 which provides a more slim design while maintaining all the features of V4. 

## Video Updates about the project

[Video Update #5](https://www.youtube.com/watch?v=LVs-HgjjFUM)

[Video Update #4](https://www.youtube.com/watch?v=E8-lO0pQunM)

[Video Update #3](https://www.youtube.com/watch?v=vvuce4NQqrk)

[Video Update #2](https://www.youtube.com/watch?v=UNLAq5FuJq4)

[Video Update #1](https://www.youtube.com/watch?v=IoVqdB_q3pE)

