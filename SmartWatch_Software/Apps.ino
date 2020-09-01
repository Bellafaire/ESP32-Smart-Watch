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
/*  Primary Area for selecting apps from within the watch. Apps take full control of the device while active.
     SelectionWindow is used to select between apps/app groups

     apps are intended to be single function calls that essentially take control of the device for the duration of their use
*/

#define APP_NUMBER 4

#define READ_CALENDAR "View Calendar"
#define MEDIA_CONTROLLER "Play Spotify"
#define CALCULATOR_APP "Calculator"
#define BLUETOOTH_SERIAL_APP "Bluetooth Serial"

String appNames[APP_NUMBER] {
  READ_CALENDAR,
  MEDIA_CONTROLLER,
  CALCULATOR_APP,
  BLUETOOTH_SERIAL_APP
};


/* this ENTIRE FILE needs a rework, as it stands right now expanding
    to add more options could become a pain
*/
void openApps() {
  SelectionWindow w = SelectionWindow(0, 14, 160, 100);

  int appSelection[APP_NUMBER];

  for (int a = 0; a < APP_NUMBER; a++)
  {
    w.addOption(appNames[a]);
  }
  int selectedOption = w.focus() - 1; //the cancel option of the selection counts as option 0, since array is zero indexed we subtract

  //so that we can re-order apps at will without changing any constants we'll search the
  //string array and determine the string value selected
  if (selectedOption == 0) {
    viewCalendar(); 
    return;
  }  else if (selectedOption == 1) {
    //things to do for the media controller
    sendBLE("/play", false);
    return;
  } else if (selectedOption == 2) {
    currentPage = CALCULATOR;
    return;
  }
  else if (selectedOption == 3) {
    currentPage = BLUETOOTH_SERIAL;
    BluetoothSerialApp();
    return;
  }
}
