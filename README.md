# ESP32 Smartwatch
This project is a fully open source smartwatch based around the ESP32, the project is ongoing for updates see the hackaday.io page for this project (https://hackaday.io/project/168227-custom-smartwatch).
The goal of this project is to create a smartwatch that is easy to reprogram and use as an IOT controller or just as a DIY fashion statement.  
Serial communication and charging are handled on-board through a single micro-usb connector without any external dock. 
The watch is built around the ESP32 WROOM module and is programable using the espressif or Arduino IDE. 
Using the ESP32 allows for the user to develop their software while leveraging the open-source libraries and examples that are available online for quick development.

The companion android app that allows the smartwatch to control media and obtain phone notifications has been split to a seperate repo and can be found [here](https://github.com/Bellafaire/Android-Companion-App-For-BLE-Devices) along with an example sketch for bluetooth interfacing using the ESP32. 

There are two version of the firmware available, the V1 firmware uses deepsleep to conserve power and requires a touch wakeup in order to be used. 
I'm not planning on doing any further work to the V1 version but the code still works so it will remain in this repo for anyone who wants to use it. 

The V2 firmware is overall much better and reccomended as the default firmware its features include: 
- light sleep, allowing for faster wakeup
- accelerometer wakeup 
- compatibility with the newer version of the android companion app 
- higher reliability in forming BLE connections (with V2 companion app)
- improved UI 

<img src="https://github.com/Bellafaire/ESP32-Smart-Watch/blob/master/Pictures%20and%20Videos/IMG_20200818_204732.jpg?raw=true" height="600px"/>

The case is 3D printed and designed in FreeCAD, the design files are available in this repo. 

## Video Updates about the project

[Video Update #4](https://www.youtube.com/watch?v=E8-lO0pQunM)

[Video Update #3](https://www.youtube.com/watch?v=vvuce4NQqrk)

[Video Update #2](https://www.youtube.com/watch?v=UNLAq5FuJq4)

[Video Update #1](https://www.youtube.com/watch?v=IoVqdB_q3pE)

