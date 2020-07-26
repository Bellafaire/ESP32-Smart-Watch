/*  Primary Area for selecting apps from within the watch. Apps take full control of the device while active.
     SelectionWindow is used to select between apps/app groups

     apps are intended to be single function calls that essentially take control of the device for the duration of their use
*/

#define APP_NUMBER 2

#define MEDIA_CONTROLLER "Media Controller"
#define BLUETOOTH_SERIAL_RECEIVIER "BT Serial"

String appNames[APP_NUMBER] {
  MEDIA_CONTROLLER,
  BLUETOOTH_SERIAL_RECEIVIER
};


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

  if (appNames[selectedOption].equals(MEDIA_CONTROLLER)) {
    //things to do for the media controller
  } else if (appNames[selectedOption].equals(BLUETOOTH_SERIAL_RECEIVIER)) {
    //things to do for the bluetooth serial receivier
  }


}
