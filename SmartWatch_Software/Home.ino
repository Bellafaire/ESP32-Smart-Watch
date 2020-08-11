bool firstHomeSwitch = true;

bool songCheckLaunched = false;
bool checkedIsPlaying = false;

double songNameScrollPosition = 0;

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
      frameBuffer->println(parseFromNotifications(a, 0) + "-" + parseFromNotifications(a, 1));
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

  //  //read the current song from the android device
  //  if (connected) {
  //
  //    if (nextButtonPressed) {
  //      pRemoteCharacteristic->writeValue("/nextSong", 9);
  //      nextButtonPressed = false;
  //    }
  //    if (lastButtonPressed) {
  //      pRemoteCharacteristic->writeValue("/lastSong", 9);
  //      lastButtonPressed = false;
  //    }
  //    if (playButtonPressed) {
  //      pRemoteCharacteristic->writeValue("/play", 5);
  //      playButtonPressed = false;
  //    }
  //    if (pauseButtonPressed) {
  //      pRemoteCharacteristic->writeValue("/pause", 6);
  //      pauseButtonPressed = false;
  //    }
  //
  //
  //    String command = "/currentSong";
  //    pRemoteCharacteristic->writeValue(command.c_str(), command.length());
  //    bool completeString = false;
  //    String songData = "";
  //    do {
  //      //get characteristic read string
  //      songData += pRemoteCharacteristic->readValue().c_str();
  //
  //      //use when needed, spams way too much data to the serial terminal
  //      //#ifdef DEBUG
  //      //      Serial.println("Current Song Data: " + songData);
  //      //#endif
  //
  //      if (songData[songData.length() - 1 ] == '*') {
  //        completeString = true;
  //
  //        //kind of an arbitrary choice for the expected length of a song + artist name but should be sufficent
  //        if (songData.length() > 10) {
  //          isPlaying = true;
  //          for (int a = 0; a < SONG_NAME_BUFFER_SIZE; a++) {
  //            if (songData[a] == '*') {
  //              songName[a] = ' ';
  //            } else {
  //              songName[a] = songData[a];
  //            }
  //          }
  //        } else {
  //          isPlaying = false;
  //        }
  //      }
  //    } while (!completeString);
  //  }

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
      songNameScrollPosition += 0.3;
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
    if (getBatteryCurrent() > 0.0) {
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
    printDebug("Last Song Button Pressed");
  }
  else if (checkButtonPress(nextSongButton, p.xPos, p.yPos)) {
    printDebug("Next Song Button Pressed");
  }
  else if (checkButtonPress(playButton, p.xPos, p.yPos)) {
    printDebug("Play Button Pressed");
  }
  else if (checkButtonPress(pauseButton, p.xPos, p.yPos)) {
    printDebug("Pause Button Pressed");
  }
}
