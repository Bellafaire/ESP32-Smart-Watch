/*  Primary Area for selecting apps from within the watch. Apps take full control of the device while active.
     SelectionWindow is used to select between apps/app groups
*/


#define APP_NUMBER 2

String appNames[APP_NUMBER] {
  "App 1",
  "App 2"
};


void openApps() {
  SelectionWindow w = SelectionWindow(0, 14, 160, 100);

  int appSelection[APP_NUMBER];

  for (int a = 0; a < APP_NUMBER; a++)
  {
    w.addOption(appNames[a]);
  }
  int selectedOption = w.focus() - 1; //the cancel option of the selection counts as option 0, since array is zero indexed we subtract

  Window e = Window(30, 40, SCREEN_WIDTH - 60, SCREEN_HEIGHT - 80, false);
  e.print("Selected Option: " + String(selectedOption)); 
  e.focus();
  return;
}
