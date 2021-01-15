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

/* Pages are what the user ultimately engages the most with, each page should have an init() function
    and a normal run function. The code is set up in such a way that for a page to be redrawn the programmer
    only needs to set the void pointer "currentPage" to the current function to be run on the screen. This way
    pages have minimal interaction with eachother and can more-or-less be written independently.

*/


//animation circles, I kind of made these awhile ago as a cool visual element but now they're fairly essential to the entire UI design
AnimationCircle circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 3.5, 2);
AnimationCircle circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -3, 3);
AnimationCircle circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 2.5, 4);
AnimationCircle circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -2, 5);
AnimationCircle circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 3, RGB_TO_BGR565(150, 150, 150), RGB_TO_BGR565(0, 0, 255), 1.5, 6);

RoundButton homeButton, settingButton, notificationsButton, homeMediaButton, homeNextMediaButton, homePreviousMediaButton, homePauseMediaButton, upButton, downButton, okButton, leftButton;


/********************************************************************
                                 HOME
 ********************************************************************/
int homeTouchArea;


//initalizes the home page
void initHome() {
  printDebug("initializing home");

  //re-init animation circles
  circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 3.5, 2);
  circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -3, 3);
  circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 2.5, 4);
  circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -2, 5);
  circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 3, RGB_TO_BGR565(150, 150, 150), RGB_TO_BGR565(0, 0, 255), 1.5, 6);

  //remove all previous touch areas, this should be done in every init function
  deactivateAllTouchAreas();

  //create touch area for going to the navigation page
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) transitionToNav);
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)initHomeMedia);
  homeMediaButton.deactivate();

  //set current page to home, initialization is complete so we're good to start drawing the normal home page
  currentPage = (void*)home;
}

//we don't want to spam the phone if we can help it, polling is easier so lets check every second
long lastSongCheck = 0;
#define SONG_CHECK_INTERVAL 1000 //how many miliseconds between checking the current song

//draws the home screen
void home() {

  //if we're connected and haven't updated our notification data then lets do so
  if (connected && notificationData.length() < 10) {
    notificationData = sendBLE("/notifications", true); //gets current android notifications as a string

    //if successful then parse out the time
    if (notificationData.length() > 10) {
      updateTimeFromNotificationData(notificationData);
    }
  }

  //draw the background image declared in Declarations.h
  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  //animate all the circles (the class will handle all of that for us as long as we call it)
  circ5.animateAndDraw(frameBuffer);
  circ4.animateAndDraw(frameBuffer);
  circ3.animateAndDraw(frameBuffer);
  circ2.animateAndDraw(frameBuffer);
  circ1.animateAndDraw(frameBuffer);

  //draw time and date (final parameter indicates shadow distance)
  drawTime(3, 5, 2, 0xFFFF, 1);
  drawDateCentered(20, 1);

  //  draw battery percentage with a slight shadow to make it pop from the background
  frameBuffer->setTextColor(0x0000);
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4, 5 );
  frameBuffer->println(String(batteryPercentage) + "%");

  frameBuffer->setTextColor(0xFFFF);
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4 - 1, 5 - 1);
  frameBuffer->println(String(batteryPercentage) + "%");

  //draw the notification data to the screen
  drawNotifications(notificationData, 0, 30, 0xFFFF);

  //display connection status on the innermost circle,
  //if we are connected also check the media playback status and use that to determine whether or not
  //we should display the media control button
  if (connected) {
    circ1.setColor(RGB_TO_BGR565(0, 255, 0));

    if (lastSongCheck + SONG_CHECK_INTERVAL < millis()) {
      boolean isPlaying = sendBLE("/isPlaying", true).substring(0, 4).equals("true");
      if (isPlaying) {
        //spotify is playing so show the button.
        homeMediaButton.activate();
      }
      lastSongCheck = millis();
    }
  } else if (deviceFound) {
    circ1.setColor(RGB_TO_BGR565(0, 0, 255));
  } else {
    circ1.setColor(RGB_TO_BGR565(255, 0, 0));
  }

  //draw the media play button (if the button is deactivated then the button will not be drawn)
  homeMediaButton.draw(frameBuffer);
}


/* Quick and dirty animation to transition to the navigation page.
  we essentially expand one of the circles until it fills the entire screen
*/
void homeTransitionToNav() {
  for (int a = 0; a < 8; a++) {
    home();
    frameBuffer->fillCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, (int)(45.0 * ((float)a / 8.0)), 0x0000);
    circ5.animateAndDraw(frameBuffer);
    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  for (int a = 0; a < 8; a++) {
    home();
    frameBuffer->fillCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45 + a * (SCREEN_WIDTH / 8), 0x0000);
    circ5.setRadius(45 + a * (SCREEN_WIDTH / 8));
    circ5.animateAndDraw(frameBuffer);
    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  }
  currentPage = (void*)initNavigation;
}

//initalizes the navigation transition from the homescreen
void transitionToNav() {
  currentPage = (void*)homeTransitionToNav;
}

/* Home Media
   technically just an extension of home activated by tapping the play icon
*/

void initHomeMedia() {
  //set the page to homeMedia
  currentPage = (void*)homeMedia;

  //clear all touch areas since we don't need them while the media controller is active
  deactivateAllTouchAreas();

  //create all the buttons we need for the media control
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)playMusic);
  homeNextMediaButton = RoundButton(56, SCREEN_HEIGHT - 56, 16, MEDIA_NEXT_ICON, (void*)nextSong);
  homePreviousMediaButton = RoundButton(16, SCREEN_HEIGHT - 66, 16, MEDIA_LAST_ICON, (void*)lastSong);
  homePauseMediaButton = RoundButton(66, SCREEN_HEIGHT - 16, 16, MEDIA_PAUSE_ICON, (void*)pauseMusic);

  //create new touch area which is used to close the media controls, this is placed directly on top of the circles of the homescreen
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) initHome);
}

//draw the home Media controls, in this case it basically just adds to the home screen so we can basically just draw on top of the home function
void homeMedia() {
  home();
  homeNextMediaButton.draw(frameBuffer);
  homePreviousMediaButton.draw(frameBuffer);
  homePauseMediaButton.draw(frameBuffer);
  homeMediaButton.draw(frameBuffer);
}



/********************************************************************
                              Navigation
 ********************************************************************/

//initalizes the navigation page with the appropriate buttons
void initNavigation() {
  deactivateAllTouchAreas();

  //create homeButton
  homeButton = RoundButton(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 16, HOME_ICON, (void*)initHome);
  settingButton = RoundButton(25, 35, 16, SETTINGS_ICON, (void*)initSettings);
  notificationsButton = RoundButton(75, 35, 16, NOTIFICATIONS_ICON, (void*)initNotifications);

  currentPage = (void*)navigation;
}

//normal animations and stuff when navigation is active.
void navigation() {
  frameBuffer->fillScreen(0x0000);
  frameBuffer->setCursor(0, 0);
  frameBuffer->println("Navigation");

  frameBuffer->setCursor(SCREEN_WIDTH - 36, SCREEN_HEIGHT - 75);
  frameBuffer->println("Home");

  //draw the animation circles
  circ4.animateAndDraw(frameBuffer);
  circ3.animateAndDraw(frameBuffer);
  circ2.animateAndDraw(frameBuffer);
  circ1.animateAndDraw(frameBuffer);

  homeButton.draw(frameBuffer);
  settingButton.draw(frameBuffer);
  notificationsButton.draw(frameBuffer);
}

/********************************************************************
                              Settings
 ********************************************************************/
void initSettings() {
  //this entire function needs to more-or-less run on its own without the framework around it
  pauseTouchAreas();
  drawInLoop = false;

  printDebug("Entering Settings");
  frameBuffer->setCursor(0, 100);

  //create selection window of the required size to display on screen.
  SelectionWindow w2 = SelectionWindow(0, 14, 160, 100);

  //options for the selection window
  w2.addOption("DayLights Savings");

  int option = w2.focus() - 1;

  switch (option) {
    //configure daylights savings
    case 0:
      {
        SelectionWindow w3 = SelectionWindow(0, 14, 160, 100);
        w3.addOption("no");
        w3.addOption("yes");
        setDataField((byte)(w3.focus() - 1), DAYLIGHT_SAVINGS);
      }
      break;
    default:
      break;
  }

  drawInLoop = true;
  printDebug("Exiting Settings");
  resumeTouchAreas();
  currentPage = (void*)initNavigation;
}


/********************************************************************
                            Notifications
 ********************************************************************/
int notificationScrollPosition = 0;

void initNotifications() {
  frameBuffer->fillScreen(BACKGROUND_COLOR);
  notificationScrollPosition = 0;

  deactivateAllTouchAreas();

  upButton = RoundButton(SCREEN_WIDTH - 20, 15, 14, UP_ARROW_ICON, (void*)lastNotification);
  okButton = RoundButton(SCREEN_WIDTH - 20, 45, 14, CHECK_MARK_ICON, (void*)showNotification);
  downButton = RoundButton(SCREEN_WIDTH - 20, 75 , 14, DOWN_ARROW_ICON, (void*)nextNotification);
  homeButton = RoundButton(SCREEN_WIDTH - 20, 105, 14, HOME_ICON, (void*)initHome);

  leftButton = RoundButton(15, 15, 14, LEFT_ARROW_ICON, (void*)initNotifications);
  leftButton.deactivate();

  currentPage = (void*)notifications;
}

void showNotification() {

  currentPage = (void*)notificationDisplay;

  deactivateAllTouchAreas();

  //activate the return button
  leftButton.activate();

}

void notificationDisplay() {
  frameBuffer->fillScreen(BACKGROUND_COLOR);
  String line = getValue(notificationData, '\n', notificationScrollPosition);

  frameBuffer->setTextColor(INTERFACE_COLOR);

  //draw title field
  frameBuffer->setCursor(35, 3);
  frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 0));

  //draw subtitle field
  frameBuffer->setCursor(35, 3 + WINDOW_CHARACTER_HEIGHT);
  frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 1));

  //draw description
  frameBuffer->setTextColor(GRAYED);
  frameBuffer->setCursor(0, 35);
  frameBuffer->println("Description");
  frameBuffer->setTextColor(INTERFACE_COLOR);
  frameBuffer->println(getValue(line, FIELD_SEPARATOR, 1));

  //draw return button
  leftButton.draw(frameBuffer);
}

void nextNotification() {
  if (notificationScrollPosition < getNumberOfLines(notificationData) - 2) {
    notificationScrollPosition++;
  }
}

void lastNotification() {
  if (notificationScrollPosition > 0) {
    notificationScrollPosition--;
  }
}


void notifications() {

  //count lines
  int lineCount = getNumberOfLines(notificationData);

  //last 2 lines are not notification data, they're time and EOM terminator
  lineCount = lineCount - 1;

  for (int a = 0; a < lineCount; a++) {
    //choose position for the current line
    frameBuffer->setCursor(0, a * WINDOW_CHARACTER_HEIGHT);

    //if the current item is selected then draw a white box behind it and make the text black
    //if the current item is not selected then treat it normally
    if (a == notificationScrollPosition) {
      frameBuffer->fillRect(0, a * WINDOW_CHARACTER_HEIGHT, SCREEN_WIDTH, WINDOW_CHARACTER_HEIGHT , INTERFACE_COLOR);
      frameBuffer->setTextColor(BACKGROUND_COLOR);
    } else {
      frameBuffer->fillRect(0, a * WINDOW_CHARACTER_HEIGHT,  SCREEN_WIDTH, WINDOW_CHARACTER_HEIGHT, BACKGROUND_COLOR);
      frameBuffer->setTextColor(INTERFACE_COLOR);
    }

    //whole lot of string operations, we only really want the very first item.
    String line = getValue(notificationData, '\n', a);
    frameBuffer->println(getValue(getValue(line, FIELD_SEPARATOR, 0), ',', 0));
  }

  okButton.draw(frameBuffer);
  upButton.draw(frameBuffer);
  homeButton.draw(frameBuffer);
  downButton.draw(frameBuffer);
}
