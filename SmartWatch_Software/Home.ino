void switchToHome()
{
  quickDrawTime(13, 10, 2);
  
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
  homeLoop();
}

void writeNotifications() {
  frameBuffer->setCursor(0, 40);
  frameBuffer->setTextSize(0);
  frameBuffer->setTextWrap(false);

  int lines = getNotificationLines();
  frameBuffer->println("Notifications");

  if (lines > 1) {
    for (int a = 1; a < lines - 1; a++) {
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

  paintButtonFull(homeNotificationsButton);
  paintButtonFull(homeAppsButton);
  paintButtonFull(homeSettingsButton);

}

int getNotificationLines() {
  int lineCount = 0;
  for (int a = 0; a < 2048; a++) {
    if (notificationData[a] == '\n') {
      lineCount++;
    }
    if (notificationData[a] == '*' &&
        notificationData[a - 1] == '*' &&
        notificationData[a - 2] == '*') {
      break;
    }
  }
  return lineCount;
}

//find the information contained in a given line and given position in a comma seperated value
String parseFromNotifications(int line, int field) {
  String lineData = getValue(String(notificationData), '\n', line);
  return getValue(lineData, ',', field);
}

//duplicated in settings.ino
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void drawHome()
{

  frameBuffer -> fillScreen(BACKGROUND_COLOR);

  frameBuffer->drawFastHLine(0, 5, frameBuffer->width(), INTERFACE_COLOR);
  drawTime(13, 10, 2);
  frameBuffer->drawFastHLine(0, 29, frameBuffer->width(), INTERFACE_COLOR);
  drawDateCentered(26, 1);
  frameBuffer->setTextSize(1);
  frameBuffer->setCursor(0, 40);

  writeNotifications() ;

  frameBuffer->fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH - 33, 10, BACKGROUND_COLOR);
  frameBuffer->setCursor(0, SCREEN_HEIGHT - 10);
  frameBuffer->print("Battery ");
  frameBuffer->print(String(getBatteryPercentage()));
  frameBuffer->print("%");
  if (getBatteryCurrent() > 0.0) {
    frameBuffer->print(" Charging");
  }

  paintButton(homeNotificationsButton);
  paintButton(homeAppsButton);
  paintButton(homeSettingsButton);

  tft.drawRGBBitmap (0, 0, frameBuffer -> getBuffer (), SCREEN_WIDTH, SCREEN_HEIGHT);
}

void HomeTouchHandler(struct point p)
{
  if (checkButtonPress(homeNotificationsButton, p.xPos, p.yPos))
  {
    pressButton(homeNotificationsButton);
    // switchToNotifications();
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
    for (int a = 0; a < 100; a++)
    {
      handleTouch();
      delay(10);
    }
    drawHome();
  }
}
