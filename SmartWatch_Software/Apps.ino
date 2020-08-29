/*  Primary Area for selecting apps from within the watch. Apps take full control of the device while active.
     SelectionWindow is used to select between apps/app groups

     apps are intended to be single function calls that essentially take control of the device for the duration of their use
*/

#define APP_NUMBER 3

#define MEDIA_CONTROLLER "Play Spotify"
#define CALCULATOR_APP "Calculator"
#define BLUETOOTH_SERIAL_APP "Bluetooth Serial"

String appNames[APP_NUMBER] {
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
    //things to do for the media controller
    sendBLE("/play", false);
    return;
  } else if (selectedOption == 1) {
    currentPage = CALCULATOR;
    return;
  }
  else if (selectedOption == 2) {
    currentPage = BLUETOOTH_SERIAL;
    BluetoothSerialApp();
    return;
  }
}
