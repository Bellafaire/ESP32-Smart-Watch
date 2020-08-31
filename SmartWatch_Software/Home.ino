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
bool firstHomeSwitch = true;

double songNameScrollPosition = 0;

long lastSongCheck = 0;
#define SONG_CHECK_INTERVAL 500 //how many miliseconds between checking the current song

//record user input before the BLE connection is established so that when the connection is
//made we can send the required commands
bool nextButtonPressed = false;
bool lastButtonPressed = false;
bool playButtonPressed = false;
bool pauseButtonPressed = false;


void writeNotifications() {
  frameBuffer->setCursor(0, 40);
  frameBuffer->setTextSize(0);
  frameBuffer->setTextWrap(false);

  int lines = getNotificationLines();
  frameBuffer->println("Notifications");

  if (lines > 1) {
    for (int a = 0; a < lines - 1; a++) {
      frameBuffer->println(parseFromNotifications(a, 0));
    }
  } else {
    frameBuffer->println("No Notifications");
  }
  frameBuffer->setTextWrap(true);

#ifdef SHOW_LAST_NOTIFICATION_TIME
  frameBuffer->setTextColor(GRAYED);
  frameBuffer->setCursor(2, SCREEN_HEIGHT - 20);
  frameBuffer->println(parseFromNotifications(lines - 1, 0));
  frameBuffer->setTextColor(INTERFACE_COLOR);
#endif

}

int circlePosition = 0;

void drawHome()
{

  frameBuffer -> drawRGBBitmap(0, 0, background, SCREEN_WIDTH, SCREEN_HEIGHT);

  //check if we've got a song playing on spotify at the moment, but only once every SONG_CHECK_INTERVAL miliseconds 
  if (lastSongCheck + SONG_CHECK_INTERVAL < millis()) {
    isPlaying = sendBLE("/isPlaying", true).substring(0, 4).equals("true");
    if (isPlaying) {
      String song = sendBLE("/currentSong", true);
      for (int a = 0; a < SONG_NAME_BUFFER_SIZE; a++) {
        if (song[a] == '*') {
          songName[a] = ' ';
        } else {
          songName[a] = song[a];
        }
      }
    }
    lastSongCheck = millis();
  }

  //it's here if you want it
  //    drawCircularAnimation1(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 + 30);
  //  drawArc(80, 60, 50, 10, circlePosition++, 30,  0x0011);

  frameBuffer->drawFastHLine(0, 5, frameBuffer->width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  frameBuffer->drawLine(0, 29, 10, 29, INTERFACE_COLOR);
  frameBuffer->drawLine(SCREEN_WIDTH, 29, SCREEN_WIDTH - 10, 29, INTERFACE_COLOR);
  drawDateCentered(26, 1);
  frameBuffer->setTextSize(1);
  frameBuffer->setCursor(0, 40);

  if (connected) {
    frameBuffer-> fillRect(SCREEN_WIDTH - 3, 0, 3, 3, 0b00111111 << 5); //if connected draw a green square in the corner
  } else if (myDevice) {
    frameBuffer-> fillRect(SCREEN_WIDTH - 3, 0, 3, 3, 0b00011111 << 11); //Show a blue square if we found the device
  } else {
    frameBuffer-> fillRect(SCREEN_WIDTH - 3, 0, 3, 3, 0b00011111 ); //else draw red square
  }

  writeNotifications() ;

  if (isPlaying) {
    frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
    if (songNameScrollPosition + 24 < String(songName).length()) {
      songNameScrollPosition += 0.1;
    } else {
      songNameScrollPosition = 0;
    }

    frameBuffer->setTextWrap(false);
    frameBuffer->println(String(songName).substring(round(songNameScrollPosition)));
    frameBuffer->setTextWrap(true);
    int buttonWidth = (SCREEN_WIDTH - 32) / 4;

    lastSongButton._x = 0 * buttonWidth;
    lastSongButton._y = SCREEN_HEIGHT - 42;
    lastSongButton._width = buttonWidth;
    lastSongButton._height = 32;

    pauseButton._x = 1 * buttonWidth;
    pauseButton._y = SCREEN_HEIGHT - 42;
    pauseButton._width = buttonWidth;
    pauseButton._height = 32;

    playButton._x = 2 * buttonWidth;
    playButton._y = SCREEN_HEIGHT - 42;
    playButton._width = buttonWidth;
    playButton._height = 32;

    nextSongButton._x = 3 * buttonWidth;
    nextSongButton._y = SCREEN_HEIGHT - 42;
    nextSongButton._width = buttonWidth;
    nextSongButton._height = 32;

    paintButtonFull(lastSongButton);
    paintButtonFull(pauseButton);
    paintButtonFull(playButton);
    paintButtonFull(nextSongButton);

  } else {
    frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
    frameBuffer->print("Battery ");
    frameBuffer->print(String(getBatteryPercentage()));
    frameBuffer->print("%");
    if (!digitalRead(CHG_STAT)) {
      frameBuffer->print(" Charging");
    }
  }

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeAppsButton);
  paintButtonFull(homeSettingsButton);

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void HomeTouchHandler(struct point p)
{
  if (checkButtonPress(homeNotificationsButton, p.xPos, p.yPos))
  {
    pressButton(homeNotificationsButton);
    currentPage = NOTIFICATIONS;
  }
  else if (checkButtonPress(homeAppsButton, p.xPos, p.yPos)) {
    openApps();
  }
  else if (checkButtonPress(homeSettingsButton, p.xPos, p.yPos))
  {
    pressButton(homeSettingsButton);
    currentPage = SETTINGS;
  }
  else if (checkButtonPress(lastSongButton, p.xPos, p.yPos)) {
    sendBLE("/lastSong", false);
    printDebug("Last Song Button Pressed");
  }
  else if (checkButtonPress(nextSongButton, p.xPos, p.yPos)) {
    sendBLE("/nextSong", false);
    printDebug("Next Song Button Pressed");
  }
  else if (checkButtonPress(playButton, p.xPos, p.yPos)) {
    sendBLE("/play", false);
    printDebug("Play Button Pressed");
  }
  else if (checkButtonPress(pauseButton, p.xPos, p.yPos)) {
    sendBLE("/pause", false);
    printDebug("Pause Button Pressed");
  }
}
