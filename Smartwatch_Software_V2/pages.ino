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

String notificationData = "";

//animation circles, I kind of made these awhile ago as a cool visual element but now they're fairly essential to the entire UI design
AnimationCircle circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 3.5, 2);
AnimationCircle circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -3, 3);
AnimationCircle circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 2.5, 4);
AnimationCircle circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -2, 5);
AnimationCircle circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 3, RGB_TO_BGR565(150, 150, 150), RGB_TO_BGR565(0, 0, 255), 1.5, 6);

RoundButton homeButton, settingButton, notificationsButton, homeMediaButton, homeNextMediaButton, homePreviousMediaButton, homePauseMediaButton;


/********************************************************************
                                 HOME
 ********************************************************************/
//ta = createTouchArea(0, 0, 50, 50, (void*)testFunction);

int homeTouchArea;

void initHome() {

  printDebug("initializing home");
  //re-init animation circles
  circ1 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 20, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 3.5, 2);
  circ2 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 25, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -3, 3);
  circ3 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 31, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), 2.5, 4);
  circ4 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 38, 3, RGB_TO_BGR565(10, 10, 10), RGB_TO_BGR565(0, 0, 0), -2, 5);
  circ5 = AnimationCircle(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 45, 3, RGB_TO_BGR565(150, 150, 150), RGB_TO_BGR565(0, 0, 255), 1.5, 6);

  //remove all previous touch areas
  deactivateAllTouchAreas();

  //create touch area for going to the navigation page
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) transitionToNav);
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)initHomeMedia);
  homeMediaButton.deactivate();
  currentPage = (void*)home;
}

long lastSongCheck = 0;
#define SONG_CHECK_INTERVAL 1000 //how many miliseconds between checking the current song

void home() {

  if (connected && notificationData.length() < 10) {
    notificationData = sendBLE("/notifications", true); //gets current android notifications as a string
    if (notificationData.length() > 10) {
      updateTimeFromNotificationData(notificationData);
    }
  }

  frameBuffer->drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  drawTime(3, 5, 2, 0xFFFF, 1);
  drawDateCentered(20, 1);

  frameBuffer->setTextColor(0x0000);
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4, 5 );
  frameBuffer->println(String(batteryPercentage) + "%");

  frameBuffer->setTextColor(0xFFFF);
  frameBuffer->setCursor(SCREEN_WIDTH - 6 * 4 - 1, 5 - 1);
  frameBuffer->println(String(batteryPercentage) + "%");

  drawNotifications(notificationData, 0, 30, 0xFFFF);

  circ5.animateAndDraw(frameBuffer);
  circ4.animateAndDraw(frameBuffer);
  circ3.animateAndDraw(frameBuffer);
  circ2.animateAndDraw(frameBuffer);
  circ1.animateAndDraw(frameBuffer);

  if (connected) {
    circ1.setColor(RGB_TO_BGR565(0, 255, 0));

    if (lastSongCheck + SONG_CHECK_INTERVAL < millis()) {
      boolean isPlaying = sendBLE("/isPlaying", true).substring(0, 4).equals("true");
      if (isPlaying) {
        homeMediaButton.activate();
      }
      lastSongCheck = millis();
    }
  } else if (myDevice) {
    circ1.setColor(RGB_TO_BGR565(0, 0, 255));
  } else {
    circ1.setColor(RGB_TO_BGR565(255, 0, 0));
  }

  homeMediaButton.draw(frameBuffer);
}


/* Quick and dirty animation to transition to the navigation page.*/
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

void transitionToNav() {
  currentPage = (void*)homeTransitionToNav;
}

/* Home Media
   technically just an extension of home activated by tapping the play icon
*/

void initHomeMedia() {
  currentPage = (void*)homeMedia;
  deactivateAllTouchAreas();
  homeMediaButton = RoundButton(16, SCREEN_HEIGHT - 16, 16, MEDIA_PLAY_ICON, (void*)playMusic);
  homeNextMediaButton = RoundButton(56, SCREEN_HEIGHT - 56, 16, MEDIA_NEXT_ICON, (void*)nextSong);
  homePreviousMediaButton = RoundButton(16, SCREEN_HEIGHT - 66, 16, MEDIA_LAST_ICON, (void*)lastSong);
  homePauseMediaButton = RoundButton(66, SCREEN_HEIGHT - 16, 16, MEDIA_PAUSE_ICON, (void*)pauseMusic);
  homeTouchArea = createTouchArea(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50, 100, 100, (void*) initHome);
}

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

void initNavigation() {
  deactivateAllTouchAreas();

  //create homeButton
  homeButton = RoundButton(SCREEN_WIDTH - 25, SCREEN_HEIGHT - 25, 16, HOME_ICON, (void*)initHome);
  settingButton = RoundButton(25, 35, 16, SETTINGS_ICON, (void*)initSettings);
  notificationsButton = RoundButton(75, 35, 16, NOTIFICATIONS_ICON, (void*)initNotifications);

  currentPage = (void*)navigation;
}

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
  navigation();
  frameBuffer->setCursor(0, 100);
  frameBuffer->println("Settings not supported yet");

  currentPage = (void*)initSettings;
}

/********************************************************************
                            Notifications
 ********************************************************************/
void initNotifications() {
  navigation();
  frameBuffer->setCursor(0, 100);
  frameBuffer->println("Notifications not supported yet");

  currentPage = (void*)initNotifications;
}
