/*****************************************************************************
The MIT License (MIT)

Copyright (c) 2020 Matthew James Bellafaire

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
/* Bluetooth serial app

   Makes the smartwatch discoverable with the name defined below using the BluetoothSerial Library
   This is meant to allow the watch to operate as a wireless serial reciever for debugging projects
   like robots that can't be easily debugged while connected to a PC.
*/

#define DEVICE_BROADCAST_NAME "ESP32 Smartwatch"
#define MAX_LINES_ON_SCREEN 16
#define MAX_CHARACTERS_PER_LINE SCREEN_WIDTH/6
#define MAX_CHARACTERS_ON_SCREEN MAX_LINES_ON_SCREEN*MAX_CHARACTERS_PER_LINE

void BluetoothSerialApp() {
  //variable that gives control of the hardware over to this app.
  char values[MAX_CHARACTERS_ON_SCREEN] = "Waiting for Connection \n hold finger on screen for 2 seconds to exit";
  String newData = "";
  BluetoothSerial SerialBT;
  SerialBT.begin(DEVICE_BROADCAST_NAME);

  while (currentPage == BLUETOOTH_SERIAL) {
    frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);
    frameBuffer->setCursor(0, 0);
    while (SerialBT.available()) {
      char c = (char)SerialBT.read();
      newData += c;
      //only one new line at a time (this simplifies the code massively)
      if (c == '\n') {
        break;
      }
    }

    String temp = newData + values;
    for (int a = 0; a < MAX_CHARACTERS_ON_SCREEN; a++) {
      values[a] = temp[a];
    }

    newData = "";
    frameBuffer->print(String(values));

    if (!digitalRead(TOUCH_IRQ)) {
      unsigned long startTime = millis();
      printDebug("user holding finger on screen");
      while (!digitalRead(TOUCH_IRQ)) {
        if (millis() > startTime + 2000) {
          printDebug("Exiting Bluetooth Serial");
          esp_sleep_enable_timer_wakeup(1000);
          deviceSleep();
        }
      }
    }

    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
}
