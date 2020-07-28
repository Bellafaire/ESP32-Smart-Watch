bool firstHomeSwitch = true;

bool songCheckLaunched = false;
bool checkedIsPlaying = false;


void updateSong(void * pvParameters ) {
  boolean complete = false;
  String xSongName = "";

#ifdef DEBUG
  Serial.println("updating song");
#endif

  String rdata;

  rdata =  connectToServer(2000, "/currentSong", true, false);
#ifdef DEBUG
  Serial.println(rdata);
#endif
  if (rdata.length() > 3) {
#ifdef DEBUG
    Serial.println(rdata);
#endif
    xSongName = rdata;
    checkedIsPlaying = true;
    isPlaying = true;
    complete = true;
  } else {
    if (rdata.substring(0, 3).equals("***")) {
      isPlaying = false;
      checkedIsPlaying = true;
      complete = true;
    } else {
      complete = false;
#ifdef DEBUG
      Serial.println("Could not determine song, retrying");
#endif
      songCheckLaunched = false;
      vTaskDelete(xSong);
    }
  }

  for (int a = 0; a < SONG_NAME_BUFFER_SIZE; a++) {
    if (xSongName[a] == '*') {
      songName[a] = ' ';
    } else {
      songName[a] = xSongName[a];
    }
  }

#ifdef DEBUG
  Serial.println("finished updating song");
#endif
  vTaskDelete(xSong);
}



void switchToHome()
{

  //  SweepClear();
  frameBuffer->drawFastHLine(0, 5, frameBuffer->width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  frameBuffer->drawFastHLine(0, 29, frameBuffer->width(), INTERFACE_COLOR);
  drawDateCentered(26, 1);
  frameBuffer->setTextSize(1);

  writeNotifications();

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeSettingsButton);
  paintButtonFull(homeAppsButton);

  currentPage = HOME;
  //  if (firstHomeSwitch) {
  //    tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
  //    firstHomeSwitch = false;
  //  }
  homeLoop();


}

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

  if (!checkedIsPlaying && connected && !songCheckLaunched) {
    xTaskCreate( updateSong, "BLE_SONG_UPDATE", 8192, (void *) 1 , tskIDLE_PRIORITY + 1, &xSong );
    songCheckLaunched = true;
    configASSERT( xSong );
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
  } else {
    frameBuffer-> fillRect(SCREEN_WIDTH - 3, 0, 3, 3, 0b00011111 ); //else draw red square
  }


  writeNotifications() ;

  if (isPlaying) {
    frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
    frameBuffer->setTextWrap(false);
    frameBuffer->println(String(songName));
    frameBuffer->setTextWrap(true);
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
    switchToNotifications();
  }
  if (checkButtonPress(homeAppsButton, p.xPos, p.yPos)) {
    openApps();
  }
  if (checkButtonPress(homeSettingsButton, p.xPos, p.yPos))
  {
    pressButton(homeSettingsButton);
    switchToSettings();
  }
}

void homeLoop()
{
  lastTouchTime = millis();
  while (lastTouchTime + screenOnTime > millis())
  {
    drawHome();
    if (touchDetected) {
      handleTouch();
      touchDetected = false;
    }

  }
}
